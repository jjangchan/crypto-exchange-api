//
// Created by jjangchan on 2024/03/30.
//

#ifndef CRYPTO_EXCHANGE_API_BINANCE_H
#define CRYPTO_EXCHANGE_API_BINANCE_H

#include "ExchangeManageMent.h"
#include <openssl/hmac.h>


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

    ~BinanceAPI(){}
    BinanceAPI(BinanceAPI&& b_api) noexcept = default;
    BinanceAPI& operator=(BinanceAPI&& b_api) noexcept = default;

    // delete
    BinanceAPI& operator=(const BinanceAPI& b_api) = delete;
    BinanceAPI(const BinanceAPI& b_api) = delete;

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

    std::string make_signed(std::string params) override{
        params.append("timestamp=");
        params.append(std::to_string(get_current_ms_epoch()));
        params.append("&recvWindow=");
        params.append(rest_pimpl->get_str_timeout());

        std::string sk = rest_pimpl->get_sk();
        std::string sign = hmac_sha256(sk.c_str(), sk.length(), params.c_str(), params.length());
        params.append("&signature=");
        params.append(sign);
        return params;
    }

    rest_result<crypto_order_send> order_send(const crypto_send_info& info) override{
        auto b_info = static_cast<const binance::send_info*>(info.get_send_info());

        const char* side = enum_side_to_str(b_info->side);
        const char* type = enum_type_to_str(b_info->type);
        const char* time = (b_info->type == binance::enum_type::market) ? nullptr : enum_time_to_str(b_info->time);

        const rest_impl::init_list_Type map = {
                 {"symbol", b_info->symbol}
                ,{"side", side}
                ,{"type", type}
                ,{"timeInForce", time}
                ,{"quantity", b_info->amount}
                ,{"price", b_info->price}
                ,{"newClientOrderId", b_info->client_order_id}
                ,{"strategyId", b_info->strategyId}
                ,{"strategyType", b_info->strategyType}
                ,{"stopPrice", b_info->stop_price}
                ,{"icebergQty", b_info->iceberg_amount}
                ,{"newOrderRespType", "FULL"}

        };
        return rest_pimpl->post(true, "/api/v3/order", boost::beast::http::verb::post, map, b_info->cb);
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
    }

private:
    std::string hmac_sha256(const char *key, std::size_t klen, const char *data, std::size_t dlen) {
        std::uint8_t digest[EVP_MAX_MD_SIZE];
        std::uint32_t dilen{};

        auto p = ::HMAC(
                ::EVP_sha256()
                ,key
                ,klen
                ,(std::uint8_t *)data
                ,dlen
                ,digest
                ,&dilen
        );
        assert(p);

        return b2a_hex(digest, dilen);
    }

    std::string b2a_hex(const std::uint8_t *p, std::size_t n) {
        static const char hex[] = "0123456789abcdef";
        std::string res;
        res.reserve(n * 2);

        for ( auto end = p + n; p != end; ++p ) {
            const std::uint8_t v = (*p);
            res += hex[(v >> 4) & 0x0F];
            res += hex[v & 0x0F];
        }

        return res;
    }
};


#endif //CRYPTO_EXCHANGE_API_BINANCE_H
