//
// Created by jjangchan on 2024/04/07.
//

#ifndef CRYPTO_EXCHANGE_API_BITHUMBAPI_H
#define CRYPTO_EXCHANGE_API_BITHUMBAPI_H

#include "ExchangeManageMent.h"


class BithumbAPI : public ExchangeManagement{
private:
public:
    using on_bithumb_cb = std::function<bool(const char* file_name,
                                             int ec,
                                             std::string err_msg,
                                             bithumb::ticker msg,
                                             const std::vector<double_type>& opens,
                                             const std::vector<double_type>& highs,
                                             const std::vector<double_type>& lows,
                                             const std::vector<double_type>& closes,
                                             const std::vector<double_type>& volumes,
                                             const std::vector<std::size_t>& open_times)
    >;
    // constructor
    BithumbAPI(boost::asio::io_context& _context,
               std::string _host,
               std::string _port,
               std::string _send_msg,
               std::size_t kline_size)
            : ExchangeManagement(_context, std::move(_host), std::move(_port), std::move(_send_msg), kline_size){}

    ~BithumbAPI(){}
    BithumbAPI(BithumbAPI&& b_api) noexcept = default;

    // delete
    BithumbAPI(const BithumbAPI& b_api) = delete;
    BithumbAPI& operator=(const BithumbAPI& b_api) = delete;
    BithumbAPI& operator=(BithumbAPI&& b_api) noexcept = delete;

public:

    bool is_api_error(const flatjson::fjson& json) override{return json.contains("status") && json.contains("resmsg");}

    std::pair<int, std::string> info_api_error(const flatjson::fjson& json) override{
        int ec = json.at("status").to_int();
        std::string msg = json.at("resmsg").to_string();
        return std::make_pair(ec, std::move(msg));
    }

    std::string make_channel(const char* symbol, const char* channel) override{
        // Bithumb raw streams -> /pub/ws
        std::string str{"/pub/ws"};
        return str;
    }

    void control_msg(Websocket& ws) override{
        ws.ws_send_msg(boost::beast::websocket::ping_data{});
    }

    static std::string make_send_msg(const char* type, const std::vector<std::string>& symbols, const std::vector<std::string>& times){
        std::string msg{"{\"type\":\""};
        msg.append(type);
        msg.append("\",\"symbols\":[");
        for(const std::string& symbol : symbols){
            std::string temp = "\""+symbol+"\",";
            msg.append(temp);
        }
        msg.pop_back();
        msg.append("],\"tickTypes\":[");
        for(const std::string& time: times){
            std::string temp = "\""+time+"\",";
            msg.append(temp);
        }
        msg.pop_back();
        msg.append("]");
        msg.append("}");
        return msg;
    }

    ExchangeManagement::handler bithumb_on_tick(const time_frame period, on_bithumb_cb cb){
        return ws_pimpl->start_websocket({}, {}, period, std::move(cb));
    }

private:
};

#endif //CRYPTO_EXCHANGE_API_BITHUMBAPI_H
