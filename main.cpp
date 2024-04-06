#include <iostream>
#include "src/BinanceAPI.h"


int main() {
    boost::asio::io_context service;
    BinanceAPI binance_api(service,"stream.binance.com", "9443", 1024);
    auto handler = binance_api.binance_on_tick("BTCUSDT", "1m", [](const char* file_name,
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
                                                       std::cerr << "websocket api error, file =" << file_name << ", ec = " << ec << ", err msg" << err_msg << std::endl;
                                                       return false;
                                                   }
                                                   std::size_t len = opens.size();
                                                   std::cout << "bar size : " << len << std::endl;
                                                   return true;
                                               }
    );

    service.run();
    return 0;
}
