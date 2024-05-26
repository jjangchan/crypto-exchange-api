#include <iostream>
#include <chrono>
#include "src/BithumbAPI.h"
#include "src/BinanceAPI.h"
#include "src/UpbitAPI.h"
#include <boost/variant.hpp>



int main() {

    boost::asio::io_context service;

    std::string send_msg = UpbitAPI::make_send_msg({
                                                           {"ticket", get_uuid().c_str()},
                                                           {"type", "ticker"},
                                                           {"codes", std::vector<std::string>{"KRW-BTC"}},
                                                           {"format", "DEFAULT"}
    });

    std::cout << send_msg << std::endl;
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
    auto handler = binance_api.binance_on_tick("BTCUSDT", time_frame::_5m, 1024, "", [](const char* file_name,
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
        std::cout << "bar size --> " << v_size << std::endl;
        //std::cout << msg << std::endl;
        return true;
    });
    **/

    service.run();
    return 0;
}
