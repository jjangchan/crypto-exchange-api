//
// Created by jjangchan on 2023/12/07.
//

#ifndef CRYPTO_EXCHANGE_API_EXCHANGEMANAGEMENT_H
#define CRYPTO_EXCHANGE_API_EXCHANGEMANAGEMENT_H


#define WS_CB_ON_ERROR(ws_cb, ec) \
    ws_cb(__FILE__ "(" BOOST_PP_STRINGIZE(__LINE__) ")", ec.value(), ec.message(), nullptr, 0);

//======================================================================================
#include <vector>
#include <functional>
#include <memory>


//======================================= boost ========================================
#include <boost/format.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>

#include <boost/callable_traits.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/intrusive/set.hpp>

//===================================== extern =========================================
#include "flatjson.hpp"
#include "ExchangeType.h"
#include "Errors.h"
//======================================================================================


/***************************************************************************************/
using namespace binance;
struct kline_t;


class Websocket : std::enable_shared_from_this<Websocket>{
private:
    friend class ExchangeManagement;
    boost::asio::io_context& asio_context;     //---> OS의 네트워크 I/O 서비스 제공
    boost::asio::ssl::context ssl_context;     //---> OpenSSL SSL_CTX 데이터 구조체를 감싸는 래퍼
    boost::asio::ip::tcp::resolver resolver;   //---> websocket server endpoint 변환 함수
    /**
     * beast::websocket::stream -> websocket 을 사용하는 메시지-지향 기능 제공
     * beast::asio::ssl::stream<boost::asio::ip::tcp::socket>
     * -> TCP socket 객체를 감싸는 스트림, 모든 SSL/TLS 통신 연산 구현
     */
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> w_socket;
    boost::beast::multi_buffer m_buffer;


    std::string host;
    std::string channel;
    bool is_on_async;
    boost::intrusive::set_member_hook<> set_m_hook;

public:
    explicit Websocket(boost::asio::io_context& _context):
        asio_context(_context),
        ssl_context(boost::asio::ssl::context::sslv23_client), //---> secure socket layer/transport layer security client
        resolver(asio_context),
        w_socket(asio_context, ssl_context),
        m_buffer{},
        host{},
        channel{},
        is_on_async{}
    {}
    virtual ~Websocket(){
        std::cout << "websocket ... delete ..." << std::endl;
    }

public:
    std::shared_ptr<Websocket> get_shared_ptr(){ return shared_from_this();}
    using ws_ptr_type = std::shared_ptr<Websocket>; // ---> shared, unique 는 소유권이 원자적 이어야함 , std::move() 사
    using on_ws_cb = std::function<
            bool(const char *file_name, int ec, std::string err_msg, const char *data, std::size_t size)>;
    void ws_start(const std::string& _host, const std::string& _port, const std::string& _channel, on_ws_cb ws_cb, ws_ptr_type ws)
    {return ws_async_start(_host, _port, _channel, std::move(ws_cb), std::move(ws));}

    void ws_stop(){
        is_on_async = true;

        // ---> websocket close
        if(w_socket.next_layer().next_layer().is_open()){
            boost::system::error_code ec;
            w_socket.close(boost::beast::websocket::close_code::normal, ec); // ---> send payload(close_code::normal)
        }
    }

private:
    void ws_async_start(const std::string& _host, const std::string& _port, const std::string& _channel, on_ws_cb ws_cb, ws_ptr_type ws){
        host = _host;
        channel = _channel;

        // ---> 항목 목록에 대한 쿼리에 정방향 분석을 비동기 수행(호스트 이름 해석)
        resolver.async_resolve(
                host,
                _port,
                [this, ws_cb=std::move(ws_cb), ws = std::move(ws)]
                        (boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type result) mutable {
                    if(ec){
                        if(!is_on_async) WS_CB_ON_ERROR(ws_cb, ec);
                    }else{
                        ws_async_connect(std::move(result), std::move(ws_cb), std::move(ws));
                    }
                });
    }

    void ws_async_connect(boost::asio::ip::tcp::resolver::results_type result, on_ws_cb ws_cb, ws_ptr_type ws){
        // ---> 접속 시도하는 서버에 domain name 세팅 함수
        if(!SSL_set_tlsext_host_name(w_socket.next_layer().native_handle(), host.c_str())){
            auto error_code = boost::beast::error_code(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category());
            WS_CB_ON_ERROR(ws_cb, error_code);
            return;
        }

        // ---> 각 EndPoint에 순차적으로 시도하여 socket 연결
        boost::asio::async_connect(
                w_socket.next_layer().next_layer(),
                result.begin(),
                result.end(),
                [this, ws_cb=std::move(ws_cb), ws = std::move(ws)]
                        (boost::system::error_code ec, boost::asio::ip::tcp::resolver::iterator) mutable {
                    if(ec){
                        if(!is_on_async) WS_CB_ON_ERROR(ws_cb, ec);
                    }else{
                        ws_async_ssl_handshake(std::move(ws_cb), std::move(ws));
                    }
                }
        );

    }

    void ws_async_ssl_handshake(on_ws_cb ws_cb, ws_ptr_type ws){
        // ---> 서버에서 연결을 확인 ping 보냄, 따라서 세션 유지 하려고 pong 응답
        w_socket.control_callback(
                [this](boost::beast::websocket::frame_type kind, boost::beast::string_view payload) mutable {
                    (void)kind; (void) payload;
                    //std::cout << "control_callback(" << this << "): kind=" << static_cast<int>(kind) << ", payload=" << payload.data() << std::endl;
                    w_socket.async_ping( // ---> send pong
                            boost::beast::websocket::ping_data{},
                            [](boost::beast::error_code ec)
                            {(void)ec;}
                    );
                }
        );

        // ---> 비동기 ssl handle shake
        w_socket.next_layer().async_handshake(
                boost::asio::ssl::stream_base::client,
                [this, ws_cb=std::move(ws_cb), ws = std::move(ws)](boost::system::error_code ec) mutable {
                    if(ec){
                       if(!is_on_async) WS_CB_ON_ERROR(ws_cb, ec);
                    }else{
                        ws_async_handshake(std::move(ws_cb), std::move(ws));
                    }
                }

        );
    }

    void ws_async_handshake(on_ws_cb ws_cb, ws_ptr_type ws){
        // ---> http websocket upgrade 비동기 요청
        w_socket.async_handshake(
                host,
                channel,
                [this,ws_cb = std::move(ws_cb), ws = std::move(ws)](boost::system::error_code ec) mutable {
                    ws_start_read(ec, std::move(ws_cb), std::move(ws));
                }
        );
    }


    void ws_start_read(boost::system::error_code ec, on_ws_cb ws_cb, ws_ptr_type ws){
        if(ec){
            if(!is_on_async) WS_CB_ON_ERROR(ws_cb, ec);
            ws_stop();
            return;
        }

        w_socket.async_read(
                m_buffer,
                [this, ws_cb = std::move(ws_cb), ws = std::move(ws)](boost::system::error_code ec, std::size_t len) mutable{
                    ws_on_read(ec, len, std::move(ws_cb), std::move(ws));
                }
        );
    }

    void ws_on_read(boost::system::error_code ec, std::size_t len, on_ws_cb ws_cb, ws_ptr_type ws){
        if(ec){
            if(!is_on_async) WS_CB_ON_ERROR(ws_cb, ec);
            ws_stop();
            return;
        }

        auto size = m_buffer.size();
        assert(size == len);

        //std::string strbuf;
        //strbuf.reserve(size);

        //for ( const auto &it: m_buffer.data() ) {
        // strbuf.append(static_cast<const char *>(it.data()), it.size());
        //}

        std::string strbuf = boost::beast::buffers_to_string(m_buffer.data());
        std::cout << strbuf << std::endl;



        m_buffer.consume(m_buffer.size()); // -> 읽어온 byte 삭제

        bool ok = ws_cb(nullptr, 0, std::string{}, strbuf.data(), strbuf.size());
        if ( !ok ) {
            ws_stop();
        } else {
            ws_start_read(boost::system::error_code{}, std::move(ws_cb), std::move(ws));
        }

    }

};

struct websocket_key_of_value{
    using type = const void*;
    // type operator()(const value_type&) const {return key_of_value}
    type operator()(const Websocket& ws) const {return std::addressof(ws);}
};

/***************************************************************************************/

class ExchangeManagement{
private:
protected:
    class ws_impl;
    //class rest_impl;

    std::unique_ptr<ws_impl> ws_pimpl;
    //std::unique_ptr<rest_impl> rest_pimpl;

public:
    // constructor
    ExchangeManagement(boost::asio::io_context& _context,
                       std::string _host,
                       std::string _port,
                       std::size_t kline_size)
            : ws_pimpl(std::make_unique<ws_impl>(*this ,_context, std::move(_host), std::move(_port), kline_size)){}
    virtual ~ExchangeManagement(){}
    ExchangeManagement(ExchangeManagement&& management) noexcept = default;

    // delete
    ExchangeManagement(const ExchangeManagement& management) = delete;
    ExchangeManagement& operator=(const ExchangeManagement& management) = delete;
    ExchangeManagement& operator=(ExchangeManagement&& management) noexcept = delete;




public:
    virtual bool is_api_error(const flatjson::fjson& json) = 0;
    virtual std::pair<int, std::string> info_api_error(const flatjson::fjson& json) = 0;
    virtual std::string make_channel(const char* pair, const char* channel) = 0;

    using handler = void*;

private:
};




class ExchangeManagement::ws_impl{
private:
    ExchangeManagement& exchange;
    // ===================================================================================
    // =                                websocket data                                   =
    boost::asio::io_context& asio_context;
    std::string host;
    std::string port;


    boost::intrusive::set<
            Websocket,
            boost::intrusive::key_of_value<websocket_key_of_value>,
            boost::intrusive::member_hook<
                    Websocket,
                    boost::intrusive::set_member_hook<>,
                    &Websocket::set_m_hook
            >
    > ws_set;
    // ===================================================================================

    // ===================================================================================
    // =                                tick data                                        =
    std::size_t bars_len;
    std::vector<std::size_t> open_times;
    std::vector<double_type> opens;
    std::vector<double_type> lows;
    std::vector<double_type> closes;
    std::vector<double_type> highs;
    std::vector<double_type> volumes;
    // ===================================================================================

public:
    ws_impl(ExchangeManagement& em,boost::asio::io_context& _context, std::string _host, std::string _port, std::size_t _kline_size):
        exchange(em),
        asio_context(_context),
        host(std::move(_host)),
        port(std::move(_port)),
        opens(_kline_size),
        lows(_kline_size),
        closes(_kline_size),
        highs(_kline_size),
        volumes(_kline_size),
        bars_len(_kline_size),
        ws_set{}
    {}

    template<typename F>
    ExchangeManagement::handler start_websocket(const char* pair, const char* channel, F cb){

        // call back 함수 argument type 정보를 가져온다.
        using cb_args_type = typename boost::callable_traits::args<F>::type;

        // call back 함수 4번째 argument type을 가져온다.
        using message_type = typename std::tuple_element<3, cb_args_type>::type;

        // shared_ptr<Websocket> deleter 구현
        static const auto deleter = [this](Websocket* ws){
            auto iter = ws_set.find(ws);
            if(iter != ws_set.end()) ws_set.erase(ws);
            delete ws; // -> websocket 소멸자 호출
        };

        // websocket instance 생성
        std::shared_ptr<Websocket> ws(new Websocket(asio_context), deleter);

        // websocket call_back function
        auto ws_cb = [this, channel, cb = std::move(cb)]
                (const char* file_name,
                 int ec,
                 std::string err_msg,
                 const char* data,
                 std::size_t size) -> bool
        {
            if(ec){ // ---> ws error
                try{
                    cb(file_name, ec, std::move(err_msg), message_type{}, {}, {}, {}, {}, {}, {});
                } catch (const std::exception& e){
                    std::fprintf(stderr, "[%s] -> %s \n", __MAKE_FILELINE ,e.what());
                    std::fflush(stderr);
                }
                return false;
            }
            const flatjson::fjson json{data, size};

            if(json.is_object() && exchange.is_api_error(json)){ // --> api error
                std::pair<int, std::string> error = exchange.info_api_error(json);
                int e_code = error.first;
                std::string e_msg = std::move(error.second);

                try{
                    return cb(__MAKE_FILELINE, e_code, std::move(e_msg), message_type{}, {}, {}, {}, {}, {}, {});
                }catch (const std::exception& e){
                    std::fprintf(stderr, "[%s] -> %s \n", __MAKE_FILELINE ,e.what());
                    std::fflush(stderr);
                }
            }
            try {
                // ---> stream data parsing, json to struct
                message_type msg = message_type::construct(json);
                msg.insert_tick_data(bars_len, open_times, opens, highs, lows, closes, volumes);
                std::cout << ">>>>>>>>>>>>>>> on json parse" << std::endl;

                return cb(file_name, ec, std::move(err_msg), std::move(msg), opens, highs, lows, closes, volumes, open_times);
            }catch (const std::exception& e){
                std::fprintf(stderr, "[%s] -> %s \n", __MAKE_FILELINE ,e.what());
                std::fflush(stderr);
            }

            return false;
        };

        // ---> websocket 통신
        std::string s_channel = exchange.make_channel(pair, channel);
        Websocket* ws_ptr = ws.get();

        ws_ptr->ws_start(
                host,
                port,
                std::move(s_channel),
                std::move(ws_cb),
                std::move(ws)
        );

    }

public:

private:
};


/***************************************************************************************/

#endif //CRYPTO_EXCHANGE_API_EXCHANGEMANAGEMENT_H