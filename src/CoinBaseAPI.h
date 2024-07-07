//
// Created by jjangchan on 2024/05/31.
//

#ifndef CRYPTO_EXCHANGE_API_COINBASEAPI_H
#define CRYPTO_EXCHANGE_API_COINBASEAPI_H
#include "ExchangeManageMent.h"

class CoinBaseAPI : public ExchangeManagement{
public:
    using on_coinbase_cb = std::function<bool(const char* file_name,
                                             int ec,
                                             std::string err_msg,
                                             coinbase::ticker msg,
                                             const std::vector<double_type>& opens,
                                             const std::vector<double_type>& highs,
                                             const std::vector<double_type>& lows,
                                             const std::vector<double_type>& closes,
                                             const std::vector<double_type>& volumes,
                                             const std::vector<std::size_t>& open_times)
    >;

    using candles_cb = std::function<bool(const char* file_name,
                                         int ec,
                                         std::string err_msg,
                                         coinbase::candles_t res)>;

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
    CoinBaseAPI(boost::asio::io_context& _context,
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

    ~CoinBaseAPI(){}
    CoinBaseAPI(CoinBaseAPI&& cb_api) noexcept = default;
    CoinBaseAPI& operator=(CoinBaseAPI&& cb_api) noexcept = default;

    // delete
    CoinBaseAPI& operator=(const CoinBaseAPI& cb_api) = delete;
    CoinBaseAPI(const CoinBaseAPI& cb_api) = delete;

public:

    bool is_api_error(const flatjson::fjson& json) override{return json.contains("message") || json.contains("channels");}

    std::pair<int, std::string> info_api_error(const flatjson::fjson& json) override{
        if(json.contains("channels")) return std::make_pair(0, "ok");
        std::string msg = json.at("message").to_string();
        msg.append("[");
        msg.append(msg);
        msg.append("]");
        return std::make_pair(400, std::move(msg));
    }

    std::string make_channel(const char* symbol, const char* channel) override{
        return "/";
    }

    void control_msg(Websocket& ws) override{
        ws.ws_send_pong({});
    }

    std::string make_signed() override{
        std::string _signed;

        return _signed;
    }

    using send_value_type = boost::variant<std::vector<std::string>, const char*>;
    using send_type = std::pair<const char*, send_value_type>;
    using init_send_Type = std::initializer_list<send_type>;
    static std::string make_send_msg(const init_send_Type& map){
        // --> variant value 확인
        auto is_valid_params_value = [](const send_value_type& value) -> bool{
            if(const auto *p = boost::get<const char*>(&value)) return *p != nullptr;
            if(const auto *p = boost::get<std::vector<std::string>>(&value)) return !p->empty();

            assert(!"invalid params value type");
            return false;
        };

        // --> variant value to str
        auto variant_to_string = [](const send_value_type & value) -> std::string{
            // str to str
            if(const auto *p = boost::get<const char*>(&value)) {
                std::string str;
                str.push_back(34);
                str.append(*p);
                str.push_back(34);
                return str;
            }

            // vector to str
            if(const auto *p = boost::get<std::vector<std::string>>(&value)){
                std::string str = "[";
                for(const std::string& s : *p){
                    if(str.size() > 1) str.push_back(',');
                    str.push_back(34);
                    str.append(s);
                    str.push_back(34);
                }
                str.push_back(']');
                return str;
            }
            assert(!"cant convert str");
            return "";
        };

        std::string send_msg = "{";
        for(const auto& pair : map){
            if(is_valid_params_value(pair.second)){
                if(send_msg.size() > 1) send_msg.push_back(',');
                send_msg.push_back(34);
                send_msg.append(pair.first);
                send_msg.push_back(34);
                send_msg.push_back(':');
                send_msg.append(variant_to_string(pair.second));
            }
        }
        send_msg.push_back('}');
        return send_msg;
    }

    ExchangeManagement::handler coinbase_on_tick(const char* pair,
                                                const coinbase::time_frame period,
                                                std::size_t bar_len,
                                                std::string send_msg,
                                                on_coinbase_cb on_cb,
                                                candles_cb k_cb = {}){

        const char* granularity = coinbase::time_frame_to_string(period);

        const rest_impl::init_list_Type map = {
                {"granularity", granularity},
        };

        std::string target = "/products/";
        target.append(pair);
        target.append("/candles");

        auto res = rest_pimpl->post(false, target.c_str(), boost::beast::http::verb::get, map, std::move(k_cb));
        std::size_t t_period = static_cast<std::underlying_type<coinbase::time_frame>::type>(period);
        unix_time_data time_data(t_period);

        if(!res){
            std::cerr << "coinbase rest error: " << res.err_msg << std::endl;
        }else{
            //std::cout << res.v << std::endl;
        }
        ws_pimpl->init_tick_data(bar_len);
        return ws_pimpl->start_websocket({}, {}, std::move(send_msg), time_data, std::move(on_cb), res);
    }

private:
};
#endif //CRYPTO_EXCHANGE_API_COINBASEAPI_H
