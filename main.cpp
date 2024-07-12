#include <iostream>
#include <chrono>
#include "src/BithumbAPI.h"
#include "src/BinanceAPI.h"
#include "src/UpbitAPI.h"
#include "src/CoinBaseAPI.h"
#include <boost/variant.hpp>



int main() {
    boost::asio::io_context service;
    /**
    std::string send_msg =  CoinBaseAPI::make_send_msg({
                                                              {"type", "subscribe"},
                                                              {"product_ids", std::vector<std::string>{"BTC-USD"}},
                                                              {"channels", std::vector<std::string>{"ticker"}}
                                                      });
    CoinBaseAPI coinbase_api(service,
                       "ws-feed.exchange.coinbase.com",
                       "443",
                       "api.exchange.coinbase.com",
                       "443",
                       "",
                       "",
                       10000,
                       "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36");
    auto handler = coinbase_api.coinbase_on_tick("BTC-USD", coinbase::time_frame::_1m, 1024, std::move(send_msg),
                            [](const char* file_name,
                               int ec,
                               std::string err_msg,
                               auto msg,
                               const auto& opens,
                               const auto& highs,
                               const auto& lows,
                               const auto& closes,
                               const auto& volumes,
                               const auto& open_times)-> bool{

                                if(ec){
                                    std::cerr << "websocket binance api error, file =" << file_name << ", ec = " << ec << ", err msg : " << err_msg << std::endl;
                                    return false;
                                }
                                std::size_t v_size = opens.size();
                                //std::cout << "bar size --> " << v_size << std::endl;
                                return true;
                            });
    **/


    std::string send_msg = UpbitAPI::make_send_msg({
                                                           {"ticket", get_uuid().c_str()},
                                                           {"type", "ticker"},
                                                           {"codes", std::vector<std::string>{"KRW-BTC"}},
                                                           {"format", "DEFAULT"}
    });

    UpbitAPI upbit_api(service,
                       "api.upbit.com",
                       "443",
                       "api.upbit.com",
                       "443",
                       "",
                       "",
                       10000,
                       "");
    upbit_api.upbit_on_tick("KRW-BTC", upbit::time_frame::_1m, 1024, std::move(send_msg),
                            [](const char* file_name,
                               int ec,
                               std::string err_msg,
                               auto msg,
                               const auto& opens,
                               const auto& highs,
                               const auto& lows,
                               const auto& closes,
                               const auto& volumes,
                               const auto& open_times)-> bool{

        if(ec){
            std::cerr << "websocket binance api error, file =" << file_name << ", ec = " << ec << ", err msg : " << err_msg << std::endl;
            return false;
        }
        std::size_t v_size = opens.size();
        //std::cout << "bar size --> " << v_size << std::endl;
        return true;
    });

    /**
    std::string send_msg =  BithumbAPI::make_send_msg({
            {"type", "ticker"},
            {"symbols", std::vector<std::string>{"BTC_KRW"}},
            {"tickTypes", std::vector<std::string>{"30M"}}
    });
    BithumbAPI bithumb_api(service,
                            "pubwss.bithumb.com",
                            "443",
                            "api.bithumb.com",
                            "443",
                            "",
                            "",
                            10000,
                            "");
    auto handler = bithumb_api.bithumb_on_tick("BTC_KRW", bithumb::time_frame::_1m, 1024, send_msg,
                                               [](const char* file_name,
                                                  int ec,
                                                  std::string err_msg,
                                                  auto msg,
                                                  const auto& opens,
                                                  const auto& highs,
                                                  const auto& lows,
                                                  const auto& closes,
                                                  const auto& volumes,
                                                  const auto& open_times)-> bool{

        if(ec){
            std::cerr << "websocket binance api error, file =" << file_name << ", ec = " << ec << ", err msg : " << err_msg << std::endl;
            return false;
        }
        //std::size_t v_size = opens.size();
        //std::cout << "bar size --> " << v_size << std::endl;
        //std::cout << msg << std::endl;
        return true;
    });
**/


    /**
    BinanceAPI binance_api(service,
                           "stream.binance.com",
                           "9443",
                           "api.binance.com",
                           "443",
                           "",
                           "",
                           10000,
                           "");
    auto handler = binance_api.binance_on_tick("BTCUSDT", time_frame::_1m, 1024, "", [](const char* file_name,
                                                                   int ec,
                                                                   std::string err_msg,
                                                                   auto msg,
                                                                   const auto& opens,
                                                                   const auto& highs,
                                                                   const auto& lows,
                                                                   const auto& closes,
                                                                   const auto& volumes,
                                                                   const auto& open_times)-> bool{

        if(ec){
            std::cerr << "websocket binance api error, file =" << file_name << ", ec = " << ec << ", err msg : " << err_msg << std::endl;
            return false;
        }
        std::size_t v_size = opens.size();
        return true;
    });
    boost::asio::steady_timer timer{service, std::chrono::steady_clock::now()+ std::chrono::seconds(5)};
    timer.async_wait([&bithumb_api, handler](const auto &ec){
        std::cout << "wake up......" << std::endl;
        bithumb_api.ws_all_async_close(handler);
    });
    **/
    service.run();
    return 0;
}
