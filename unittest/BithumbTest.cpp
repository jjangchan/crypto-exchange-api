//
// Created by jjangchan on 2024/07/12.
//

#include <gtest/gtest.h>
#include <iostream>
#include "../src/BithumbAPI.h"

class BithumbTest: public ::testing::Test{
public:
protected:
    BithumbTest(){}
    virtual ~BithumbTest(){}

public:
    virtual void SetUp(){
        std::cout << "SET UP BITHUMB ======================================================" << std::endl;
    }
    virtual void TearDown(){
        std::cout << "TEAR DOWN BITHUMB ======================================================" << std::endl;
    }
};

TEST_F(BithumbTest, bithumb_ws_1m_timer_test){
    boost::asio::io_context service;
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
    int is = 1;
    auto handler = bithumb_api.bithumb_on_tick("BTC_KRW", bithumb::time_frame::_1m, 1024, send_msg,
                                               [&is](const char* file_name,
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
                                                       is = 0;
                                                       std::cout << "websocket bithumb api error, file =" << file_name << ", ec = " << ec << ", err msg : " << err_msg << std::endl;
                                                       return false;
                                                   }
                                                   return true;
                                               });

    boost::asio::steady_timer timer{service, std::chrono::steady_clock::now()+ std::chrono::minutes(1)+std::chrono::seconds(1)};
    timer.async_wait([&bithumb_api, handler](const auto &ec){
        bithumb_api.ws_all_async_close(handler);
    });

    service.run();
    ASSERT_TRUE(is);
}