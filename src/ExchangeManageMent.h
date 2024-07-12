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
#include <boost/variant.hpp>

//===================================== extern =========================================
#include "flatjson.hpp"
#include "ExchangeType.h"
#include "Errors.h"
//======================================================================================


/***************************************************************************************/
using namespace binance;
class ExchangeManagement;
struct kline_t;
struct klines_t;


class Websocket : public std::enable_shared_from_this<Websocket>{
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
    std::string send_msg;
    bool is_stop_ws;
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
        send_msg{},
        is_stop_ws{}
    {}
    virtual ~Websocket(){
        std::cout << "websocket ... delete ..." << std::endl;
    }

public:
    using ws_ptr_type = std::shared_ptr<Websocket>; // ---> shared, unique 는 소유권이 원자적 이어야함 , std::move() 사
    using on_ws_cb = std::function<
            bool(const char *file_name, int ec, std::string err_msg, const char *data, std::size_t size)>;
    using control_cb = std::function<void(Websocket& ws)>;
    void ws_start(const std::string& _host, const std::string& _port, const std::string& _channel, std::string _send_msg, on_ws_cb ws_cb, control_cb cont_cb, ws_ptr_type ws)
    {return ws_async_start(_host, _port, _channel, std::move(_send_msg), std::move(ws_cb), std::move(cont_cb), std::move(ws));}

    void ws_stop(){
        is_stop_ws = true;

        // ---> websocket close
        if(w_socket.next_layer().next_layer().is_open()){
            boost::system::error_code ec;
            w_socket.close(boost::beast::websocket::close_code::normal, ec); // ---> send payload(close_code::normal)
        }
    }
    
    void ws_async_stop(){
        is_stop_ws = true;
        ws_ptr_type ws = shared_from_this();

        // ---> websocket async close
        if(w_socket.next_layer().next_layer().is_open()){
            w_socket.async_close(
                    boost::beast::websocket::close_code::normal,
                    [ws = std::move(ws)](const boost::system::error_code &){std::cout << ".......websocket close........." << std::endl;}
            );
        }
    }

    void ws_send_pong(const std::string& msg){
        w_socket.async_pong( // ---> send pong
                boost::beast::websocket::ping_data{msg},
                [](boost::beast::error_code ec){}
        );
    }

    template<typename S>
    void ws_write_msg(const S& msg){
        w_socket.async_write(
                boost::asio::buffer(msg),
                [this](boost::system::error_code ec, std::size_t rd) {}
        );
    }

private:
    void ws_async_start(const std::string& _host, const std::string& _port, const std::string& _channel, std::string _send_msg, on_ws_cb ws_cb, control_cb cont_cb, ws_ptr_type ws){
        host = _host;
        channel = _channel;
        send_msg = std::move(_send_msg);


        // ---> 항목 목록에 대한 쿼리에 정방향 분석을 비동기 수행(호스트 이름 해석)
        resolver.async_resolve(
                host,
                _port,
                [this, ws_cb=std::move(ws_cb), ws = std::move(ws), cont_cb = std::move(cont_cb)]
                        (boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type result) mutable {
                    if(ec){
                        if(!is_stop_ws) WS_CB_ON_ERROR(ws_cb, ec);
                    }else{
                        ws_async_connect(std::move(result), std::move(ws_cb), std::move(cont_cb), std::move(ws));
                    }
                });
    }

    void ws_async_connect(boost::asio::ip::tcp::resolver::results_type result, on_ws_cb ws_cb, control_cb cont_cb, ws_ptr_type ws){
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
                [this, ws_cb=std::move(ws_cb), ws = std::move(ws), cont_cb = std::move(cont_cb)]
                        (boost::system::error_code ec, boost::asio::ip::tcp::resolver::iterator) mutable {
                    if(ec){
                        if(!is_stop_ws) WS_CB_ON_ERROR(ws_cb, ec);
                    }else{
                        ws_async_ssl_handshake(std::move(ws_cb), std::move(cont_cb), std::move(ws));
                    }
                }
        );

    }

    void ws_async_ssl_handshake(on_ws_cb ws_cb, control_cb cont_cb, ws_ptr_type ws){
        // ---> 서버에서 연결을 확인 ping 보냄, 따라서 세션 유지 하려고 pong 응답
        w_socket.control_callback(
                [this, cont_cb = std::move(cont_cb)](boost::beast::websocket::frame_type kind, boost::beast::string_view payload) mutable {
                    /**(void)kind; (void) payload;**/
                    std::cout << "control_callback(" << this << "): kind=" << static_cast<int>(kind) << ", payload=" << payload.data() << std::endl;
                    cont_cb(*this);
                    /**
                    if(send_msg.empty()){
                        w_socket.async_pong( // ---> send pong
                                boost::beast::websocket::ping_data{},
                                [](boost::beast::error_code ec)
                                {(void)ec;}
                        );
                    }else{
                        w_socket.async_write( // ---> 한국 거래소는 request parameter json 형식 으로 응답 해야함
                                boost::asio::buffer(boost::beast::websocket::ping_data{}),
                                [](boost::beast::error_code ec, std::size_t t)
                                {(void)ec;}
                        );
                    }
                     **/
                }
        );

        // ---> 비동기 ssl handle shake
        w_socket.next_layer().async_handshake(
                boost::asio::ssl::stream_base::client,
                [this, ws_cb=std::move(ws_cb), ws = std::move(ws)](boost::system::error_code ec) mutable {
                    if(ec){
                       if(!is_stop_ws) WS_CB_ON_ERROR(ws_cb, ec);
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
                    //std::cout << "send msg --> " << send_msg << std::endl;
                    if(send_msg.empty()) ws_start_read(ec, std::move(ws_cb), std::move(ws));
                    else ws_send_msg(ec, std::move(ws_cb), std::move(ws));
                }
        );
    }

    void ws_send_msg(boost::system::error_code ec, on_ws_cb ws_cb, ws_ptr_type ws){
        if(ec){
            if(!is_stop_ws) WS_CB_ON_ERROR(ws_cb, ec);
            ws_stop();
            return;
        }

        w_socket.async_write(
                boost::asio::buffer(send_msg),
                [this, ws_cb = std::move(ws_cb), ws = std::move(ws)](boost::system::error_code ec, std::size_t rd) mutable{
                    ws_start_read(ec, std::move(ws_cb), std::move(ws));
                }
        );
    }

    void ws_start_read(boost::system::error_code ec, on_ws_cb ws_cb, ws_ptr_type ws){
        if(ec){
            if(!is_stop_ws) WS_CB_ON_ERROR(ws_cb, ec);
            ws_stop();
            return;
        }

        w_socket.async_read(
                m_buffer,
                [this, ws_cb = std::move(ws_cb), ws = std::move(ws)](boost::system::error_code ec, std::size_t rd) mutable{
                    ws_on_read(ec, rd, std::move(ws_cb), std::move(ws));
                }
        );
    }

    void ws_on_read(boost::system::error_code ec, std::size_t len, on_ws_cb ws_cb, ws_ptr_type ws){
        if(ec){
            if(!is_stop_ws) WS_CB_ON_ERROR(ws_cb, ec);
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
        //std::cout << "read buffer : " << strbuf << std::endl;

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
    class rest_impl;

    std::unique_ptr<ws_impl> ws_pimpl;
    std::unique_ptr<rest_impl> rest_pimpl;

public:
    // constructor
    ExchangeManagement(boost::asio::io_context& _context,
                       std::string ws_host,
                       std::string ws_port,
                       std::string rest_host,
                       std::string rest_port,
                       std::string rest_pk,
                       std::string rest_sk,
                       std::size_t rest_timeout,
                       std::string rest_client_user_agent)
            : ws_pimpl(std::make_unique<ws_impl>(*this ,
                                                 _context,
                                                 std::move(ws_host),
                                                 std::move(ws_port))),
              rest_pimpl(std::make_unique<rest_impl>(*this,
                                                     _context,
                                                     std::move(rest_host),
                                                     std::move(rest_port),
                                                     std::move(rest_pk),
                                                     std::move(rest_sk),
                                                     rest_timeout,
                                                     std::move(rest_client_user_agent)))
    {}
    virtual ~ExchangeManagement(){}
    ExchangeManagement(ExchangeManagement&& management) noexcept = default;
    ExchangeManagement& operator=(ExchangeManagement&& management) noexcept = default;

    // delete
    ExchangeManagement& operator=(const ExchangeManagement& management) = delete;
    ExchangeManagement(const ExchangeManagement& management) = delete;




public:
    // ===================================================================================
    virtual bool is_api_error(const flatjson::fjson& json) = 0;
    virtual std::pair<int, std::string> info_api_error(const flatjson::fjson& json) = 0;
    virtual std::string make_channel(const char* pair, const char* channel) = 0;
    virtual void control_msg(Websocket& ws) = 0;
    virtual std::string make_signed() = 0;
    // ===================================================================================

    using handler = void*;

    template<typename T>
    struct rest_result{
        int ec;
        std::string err_msg;
        std::string replay;
        T v;

        // --> return error when false;
        explicit operator bool() const {return err_msg.empty();}
    };

    void ws_close(const handler &h);
    void ws_async_close(const handler &h);

    void ws_all_close(const handler &h);
    void ws_all_async_close(const handler &h);

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
    ws_impl(ExchangeManagement& em,
            boost::asio::io_context& _context,
            std::string _host,
            std::string _port):
        exchange(em),
        asio_context(_context),
        host(std::move(_host)),
        port(std::move(_port)),
        ws_set{}
    {}

public:
    template<
            typename F,
            typename R
            >
    ExchangeManagement::handler start_websocket(const char* pair, const char* channel, std::string send_msg, unix_time_data& time_data, F cb,R& res){

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

        // 거래소 rest api로 가져온 bar data insert
        if(res){
            res.v.init_tick_data(bars_len, open_times, opens, highs, lows, closes, volumes);
            std::size_t i = open_times.size()-10 < 0 ? 0 : open_times.size()-10;
            for(; i < open_times.size(); i++){
                std::cout << "open_times --> " << open_times[i]/1000 << std::endl;
                std::cout << "open --> " << opens[i] << std::endl;
                std::cout << "high --> " << highs[i] << std::endl;
                std::cout << "low --> " << lows[i] << std::endl;
                std::cout << "close --> " << closes[i] << std::endl;
                std::cout << "volume --> " << volumes[i] << std::endl;
                std::cout << "==============================================================" << std::endl;
            }
        }

        // websocket call_back function
        auto ws_cb = [this, time_data, channel, cb = std::move(cb)]
                (const char* file_name,
                 int ec,
                 std::string err_msg,
                 const char* data,
                 std::size_t size) mutable -> bool
        {
            if(ec){ // ---> ws error
                try{
                    cb(file_name, ec, std::move(err_msg), message_type{}, {}, {}, {}, {}, {}, {});
                } catch (const std::exception& e){;
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
                msg.insert_tick_data(bars_len, open_times, opens, highs, lows, closes, volumes, time_data);
                return cb(file_name, ec, std::move(err_msg), std::move(msg), opens, highs, lows, closes, volumes, open_times);
            }catch (const std::exception& e){
                std::fprintf(stderr, "[%s] -> %s \n", __MAKE_FILELINE ,e.what());
                std::fflush(stderr);
            }

            return false;
        };

        // websocket ping-pong cb
        auto cont_cb = [this](Websocket& ws)->void{
            exchange.control_msg(ws);
        };

        // ---> websocket 통신
        std::string s_channel = exchange.make_channel(pair, channel);
        auto *ws_ptr = ws.get();
        ws_ptr->ws_start(
                host,
                port,
                s_channel,
                std::move(send_msg),
                std::move(ws_cb),
                std::move(cont_cb),
                std::move(ws)
        );

        // 구독된 websocket set 으로 관리
        ws_set.insert(*ws_ptr);
        return ws_ptr;
    }

    void init_tick_data(const std::size_t len){
        bars_len = len;
        open_times.clear();
        opens.clear();
        highs.clear();
        lows.clear();
        closes.clear();

        if(bars_len > open_times.capacity()){
            open_times.reserve(bars_len);
            opens.reserve(bars_len);
            highs.reserve(bars_len);
            lows.reserve(bars_len);
            closes.reserve(bars_len);
        }
    }

    void ws_stop_channel(handler h){
        return stop_channel(h, [](auto ws){ws->ws_stop();});
    }

    void ws_async_stop_channel(handler h){
        return stop_channel(h, [](auto ws){ws->ws_async_stop();});
    }


    void ws_all_async_stop_channel(handler h){
        return all_stop_channel(h, [](auto ws){ws->ws_async_stop();});
    }
    void ws_all_stop_channel(handler h){
        return all_stop_channel(h, [](auto ws){ws->ws_stop();});
    }

private:

    template<typename F>
    void stop_channel(handler h, F f){
        auto it = ws_set.find(h);
        if( it == ws_set.end()){return;} // --> not find

        auto *ws = static_cast<Websocket*>(&(*it));
        f(ws);

        ws_set.erase(it);
    }

    template<typename F>
    void all_stop_channel(handler h, F f){
        for(auto it = ws_set.begin(); it != ws_set.end();){
            auto *ws = static_cast<Websocket*>(&(*it));
            f(ws);
            it = ws_set.erase(it);
        }
    }
};

class ExchangeManagement::rest_impl{
private:
    boost::asio::io_context& asio_context;
    const std::string host;
    const std::string port;
    const std::string pk;
    const std::string sk;
    const std::size_t timeout;
    const std::string client_user_agent;
    ExchangeManagement& exchange;

    boost::asio::ssl::context ssl_context;
    boost::asio::ip::tcp::resolver resolver;
    boost::beast::flat_buffer f_buffer;

public:
    rest_impl(ExchangeManagement& _exchange,
              boost::asio::io_context& _context,
              std::string _host,
              std::string _port,
              std::string _pk,
              std::string _sk,
              std::size_t _timeout,
              std::string _client_user_agent)
              : exchange(_exchange),
              asio_context(_context),
              host{std::move(_host)},
              port{std::move(_port)},
              pk{std::move(_pk)},
              sk{std::move(_sk)},
              timeout{_timeout},
              client_user_agent{std::move(_client_user_agent)},
              ssl_context{boost::asio::ssl::context::sslv23_client},
              resolver{asio_context}
    {}
public:
    using params_value_type = boost::variant<std::size_t, const char*>;
    using params_type = std::pair<const char*, params_value_type>;
    using init_list_Type = std::initializer_list<params_type>;

    template<typename CB,
            typename Args = typename boost::callable_traits::args<CB>::type,
            typename R = typename std::tuple_element<3, Args>::type
            >
    ExchangeManagement::rest_result<R> post(bool _signed,
                                            const char* target,
                                            boost::beast::http::verb action,
                                            const std::initializer_list<params_type>& map,
                                            CB cb){

        // --> variant value 확인
        auto is_valid_params_value = [](const params_value_type& value) -> bool{
            if(const auto *p = boost::get<const char*>(&value)) return *p != nullptr;
            if(const auto *p = boost::get<std::size_t>(&value)) return *p != 0u;

            assert(!"invalid params value type");
            return false;
        };

        // --> variant value to str
        auto variant_to_string = [](char* buffer, std::size_t buffer_size, const params_value_type& value) -> const char*{
            // str to str
            if(const auto *p = boost::get<const char*>(&value)) return *p;

            // size_t to str
            if(const auto *p = boost::get<std::size_t>(&value)){
                std::snprintf(buffer, buffer_size, "%zu", *p);
                return buffer;
            }
            assert(!"cant convert str");
            return buffer;
        };

        auto is_html = [](const char *str) -> bool {
            return std::strstr(str, "<HTML>")
                   || std::strstr(str, "<HEAD>")
                   || std::strstr(str, "<BODY>")
                    ;
        };

        std::string post_url = target;
        std::string params;

        for(const auto& pair : map){
            if(is_valid_params_value(pair.second)){
                if(!params.empty()) params.append("&");
                params.append(pair.first);
                params.append("=");

                char buffer[32];
                params.append(variant_to_string(buffer, sizeof(buffer), pair.second));
            }
        }

        //std::cout << "params -> " << params << std::endl;

        if(_signed){
            assert(!pk.empty() && !sk.empty());
            if(!params.empty()) params.append("&");
            params.append(exchange.make_signed());
        }

        bool is_get_delete = (action == boost::beast::http::verb::get || action == boost::beast::http::verb::delete_);

        if(is_get_delete && !params.empty()){
            post_url.append("?");
            post_url.append(params);
            params.clear();
        }

        //std::cout << "post url -> " << post_url << std::endl;
        ExchangeManagement::rest_result<R> res{};

        if(!cb){ // --> sync post
            try{
                ExchangeManagement::rest_result<std::string> r = sync_post(post_url.c_str(), action, std::move(params));

                if(!r.v.empty() && is_html(r.v.c_str())){
                    // ---> host error
                    r.err_msg = std::move(r.v);
                }else{
                    std::string read_buffer = std::move(r.v);

                    const flatjson::fjson json(read_buffer.c_str(), read_buffer.length());
                    if(json.error() != flatjson::FJ_EC_OK){
                        // ---> json error
                        res.ec = json.error();
                        __MAKE_ERRMSG(res, json.error_string());
                        res.replay.clear();

                        return res;
                    }

                    if(json.is_object() && exchange.is_api_error(json)){
                        // ---> request error
                        auto info_error = exchange.info_api_error(json);
                        res.ec = info_error.first;
                        __MAKE_ERRMSG(res, info_error.second);
                        res.replay.clear();

                        return res;
                    }else{
                        // ---> json parsing
                        res.v = R::construct(json);
                    }
                }

            }catch (const std::exception& ex){
                __MAKE_ERRMSG(res, ex.what())
            }

            return res;
        }else{ // --> async post

        }
    }
private:
    ExchangeManagement::rest_result<std::string> sync_post(const char* post_url, boost::beast::http::verb action, std::string params){
        ExchangeManagement::rest_result<std::string> res{};

        /**
         * boost::asio::ssl::stream<boost::asio::ip::tcp::socket>
         * --> TCP socket 객체를 감싸는 스트림, 모든 SSL/TLS 통신 연산 구현
         */
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_stream(asio_context, ssl_context);

        // ---> 접속 시도하는 서버에 domain name 세팅 함수
        if(!SSL_set_tlsext_host_name(ssl_stream.native_handle(), host.c_str())){
            boost::system::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
            std::cerr << __MESSAGE("msg=" << ec.message()) << std::endl;
            __MAKE_ERRMSG(res, ec.message());
            return res;
        }

        boost::system::error_code ec;
        // ---> 항목 목록에 대한 쿼리에 정방향 분석을 동기 수행(호스트 이름 해석)
        auto const result = resolver.resolve(host, port, ec);
        if(ec){
            std::cerr << __MESSAGE("msg=" << ec.message()) << std::endl;
            __MAKE_ERRMSG(res, ec.message());
            return res;
        }

        // ---> 각 EndPoint에 순차적으로 시도하여 socket 연결
        boost::asio::connect(ssl_stream.next_layer(), result.begin(), result.end(), ec);
        if(ec){
            std::cerr << __MESSAGE("msg=" << ec.message()) << std::endl;
            __MAKE_ERRMSG(res, ec.message());
            return res;
        }

        // ---> ssl handle shake
        ssl_stream.handshake(boost::asio::ssl::stream_base::client, ec);
        if ( ec ) {
            std::cerr << __MESSAGE("msg=" << ec.message()) << std::endl;

            __MAKE_ERRMSG(res, ec.message());
            return res;
        }

        // ---> request format setting
        boost::beast::http::request<boost::beast::http::string_body> req;
        req.target(post_url);
        req.version(11); // version 1.1
        req.method(action);
        if(action != boost::beast::http::verb::get){
            req.body() = std::move(params);
            req.set(boost::beast::http::field::content_length, std::to_string(req.body().length()));
        }

        req.insert("X-MBX-APIKEY", pk);
        req.set(boost::beast::http::field::host, host);
        req.set(boost::beast::http::field::user_agent, client_user_agent);
        req.set(boost::beast::http::field::content_type, "application/x-www-form-urlencoded");

        // --> send request
        boost::beast::http::write(ssl_stream, req, ec);
        if ( ec ) {
            std::cerr << __MESSAGE("msg=" << ec.message()) << std::endl;

            __MAKE_ERRMSG(res, ec.message());
            return res;
        }

        boost::beast::flat_buffer buffer;
        boost::beast::http::response<boost::beast::http::string_body> resp;

        // --> read response
        boost::beast::http::read(ssl_stream, buffer, resp, ec);
        if ( ec ) {
            std::cerr << __MESSAGE("msg=" << ec.message()) << std::endl;

            __MAKE_ERRMSG(res, ec.message());
            return res;
        }

        res.v = std::move(resp.body());
        //std::cout << post_url << " REPLY:\n" << res.v << std::endl << std::endl;

        // --> stream 에서 ssl을 종료
        ssl_stream.shutdown(ec);

        return res;
    }
};

inline void ExchangeManagement::ws_close(const handler &h) {
    return ws_pimpl->ws_stop_channel(h);
}

inline void ExchangeManagement::ws_async_close(const handler &h) {
    return ws_pimpl->ws_async_stop_channel(h);
}

inline void ExchangeManagement::ws_all_close(const handler &h) {
    return ws_pimpl->ws_all_stop_channel(h);
}

inline void ExchangeManagement::ws_all_async_close(const handler &h) {
    return ws_pimpl->ws_all_async_stop_channel(h);
}


/***************************************************************************************/

#endif //CRYPTO_EXCHANGE_API_EXCHANGEMANAGEMENT_H
