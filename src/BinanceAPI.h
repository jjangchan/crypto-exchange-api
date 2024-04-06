//
// Created by jjangchan on 2024/03/30.
//

#ifndef MAIN_CPP_BINANCEAPI_H
#define MAIN_CPP_BINANCEAPI_H

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
    // constructor
    BinanceAPI(boost::asio::io_context& _context,
               std::string _host,
               std::string _port,
               std::size_t kline_size)
    : ExchangeManagement(_context, std::move(_host), std::move(_port), kline_size){}

    ~BinanceAPI(){}
    BinanceAPI(BinanceAPI&& b_api) noexcept = default;

    // delete
    BinanceAPI(const BinanceAPI& b_api) = delete;
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

    ExchangeManagement::handler binance_on_tick(const char* pair, const char* period, on_binance_cb cb){
            const char* channel = binance::time_frames(period);
            return ws_pimpl->start_websocket(pair, channel, std::move(cb));
    }

private:
};


#endif //MAIN_CPP_BINANCEAPI_H
