//
// Created by jjangchan on 2024/03/30.
//

#ifndef CRYPTO_EXCHANGE_API_BINANCE_H
#define CRYPTO_EXCHANGE_API_BINANCE_H

#include "ExchangeManageMent.h"


class BinanceAPI : public ExchangeManagement{
private:
public:
    using on_binance_cb = std::function<bool(const char* file_name,
                                               int ec,
                                               std::string err_msg,
                                               binance::kline_t msg,
                                               const std::vector<double_type>& opens,
                                               const std::vector<double_type>& highs,
                                               const std::vector<double_type>& lows,
                                               const std::vector<double_type>& closes,
                                               const std::vector<double_type>& volumes,
                                               const std::vector<std::size_t>& open_times)
    >;

    using klines_cb = std::function<bool(const char* file_name,
                                         int ec,
                                         std::string err_msg,
                                         binance::klines_t res)>;

    // constructor
    /**
     * @param _context     : asio context
     * @param ws_host      : websocket host name
     * @param ws_port      : websocket port
     * @param rest_host    : rest API host name
     * @param rest_port    : rest API port
     * @param rest_pk      : rest API private key
     * @param rest_sk      : rest API secret key
     * @param rest_timeout
     * @param rest_client_user_agent
     */
    BinanceAPI(boost::asio::io_context& _context,
<<<<<<< HEAD
               std::string _host,
               std::string _port,
               std::string _send_msg,
               std::size_t kline_size)
    : ExchangeManagement(_context, std::move(_host), std::move(_port), std::move(_send_msg), kline_size){}
=======
               std::string ws_host,
               std::string ws_port,
               std::string rest_host,
               std::string rest_port,
               std::string rest_pk,
               std::string rest_sk,
               std::size_t rest_timeout,
               std::string rest_client_user_agent)
    : ExchangeManagement(_context,
                         std::move(ws_host),
                         std::move(ws_port),
                         std::move(rest_host),
                         std::move(rest_port),
                         std::move(rest_pk),
                         std::move(rest_sk),
                         rest_timeout,
                         std::move(rest_client_user_agent))
    {}
>>>>>>> 5651c9286bb7a0bfecb1bfb9bb6bf3fa3a6820a5

    ~BinanceAPI(){}
    BinanceAPI(BinanceAPI&& b_api) noexcept = default;
    BinanceAPI(const BinanceAPI& b_api) = default;

    // delete
    BinanceAPI& operator=(const BinanceAPI& b_api) = delete;
    BinanceAPI& operator=(BinanceAPI&& b_api) noexcept = delete;

public:

    bool is_api_error(const flatjson::fjson& json) override{return json.contains("code") && json.contains("msg");}

    std::pair<int, std::string> info_api_error(const flatjson::fjson& json) override{
        int ec = json.at("code").to_int();
        std::string msg = json.at("msg").to_string();
        return std::make_pair(ec, std::move(msg));
    }

    std::string make_channel(const char* symbol, const char* channel) override{
        // binance raw streams -> /ws/<StreamName>
        std::string str{"/ws/"};
        str.append(symbol);
        boost::algorithm::to_lower(str);
        str.push_back('@');
        str.append(channel);
        return str;
    }

    void control_msg(Websocket& ws) override{
        ws.ws_send_pong({});
    }

<<<<<<< HEAD
    ExchangeManagement::handler binance_on_tick(const char* pair, const time_frame period, on_binance_cb cb){
            const char* channel = binance::time_frames(time_frame_to_string(period));
            return ws_pimpl->start_websocket(pair, channel, period, std::move(cb));
=======
    std::string make_signed() override{
        std::string _signed;

        return _signed;
    }

    ExchangeManagement::handler binance_on_tick(const char* pair,
                                                const binance::time_frame period,
                                                std::size_t bar_len,
                                                std::string send_msg,
                                                on_binance_cb on_cb,
                                                klines_cb k_cb = {}){

        const char* interval = binance::time_frame_to_string(period);
        const char* channel = binance::time_frames(interval);

        const rest_impl::init_list_Type map = {
                {"symbol", pair},
                {"limit", bar_len},
                {"interval", interval}
        };

        auto res = rest_pimpl->post(false, "/api/v3/klines", boost::beast::http::verb::get, map, std::move(k_cb));
        unix_time_data time_data;

        if(!res){
            std::cerr << "binance rest error: " << res.err_msg << std::endl;
        }else{
            //std::cout << res.v << std::endl;
        }
        ws_pimpl->init_tick_data(bar_len);
        return ws_pimpl->start_websocket(pair, channel, send_msg, time_data, std::move(on_cb), res);
>>>>>>> 5651c9286bb7a0bfecb1bfb9bb6bf3fa3a6820a5
    }

private:
};


#endif //CRYPTO_EXCHANGE_API_BINANCE_H
