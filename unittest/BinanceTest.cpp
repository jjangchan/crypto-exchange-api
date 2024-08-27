//
// Created by jjangchan on 2024/07/12.
//

#include <gtest/gtest.h>
#include <iostream>
#include "../src/BinanceAPI.h"

class BinanceTest: public ::testing::Test{
public:
protected:
    BinanceTest() {
    }
    virtual ~BinanceTest(){}

public:
    virtual void SetUp(){
        std::cout << "SET UP BINANCE ======================================================" << std::endl;

    }
    virtual void TearDown(){
        std::cout << "TEAR DOWN BINANCE ======================================================" << std::endl;
    }
};

TEST_F(BinanceTest, binance_ws_1m_timer_test){
    boost::asio::io_context service;
    BinanceAPI binance_api(service,
                           "stream.binance.com",
                           "9443",
                           "api.binance.com",
                           "443",
                           "",
                           "",
                           10000,
                           "");
    int is = 1;
    auto handler = binance_api.binance_on_tick("BTCUSDT", binance::time_frame::_1m, 1024, "", [&is](const char* file_name,
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
            std::cout << "websocket binance api error, file =" << file_name << ", ec = " << ec << ", err msg : " << err_msg << std::endl;
            is = 0;
            return false;
        }
        return true;
    });

    boost::asio::steady_timer timer{service, std::chrono::steady_clock::now()+ std::chrono::minutes(1)+std::chrono::seconds(1)};
    timer.async_wait([&binance_api, handler](const auto &ec){
        binance_api.ws_all_async_close(handler);
    });

    service.run();
    ASSERT_TRUE(is);
}
