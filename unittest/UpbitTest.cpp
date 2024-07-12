//
// Created by jjangchan on 2024/07/12.
//

#include <gtest/gtest.h>
#include <iostream>
#include "../src/UpbitAPI.h"

class UpbitTest: public ::testing::Test{
public:
protected:
    UpbitTest(){}
    virtual ~UpbitTest(){}

public:
    virtual void SetUp(){
        std::cout << "SET UP" << std::endl;

    }
    virtual void TearDown(){
        std::cout << "TEAR DOWN" << std::endl;
    }
};

TEST_F(UpbitTest, upbit_ws_1m_timer_test){
    boost::asio::io_context service;
    int is = 1;

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
    auto handler = upbit_api.upbit_on_tick("KRW-BTC", upbit::time_frame::_1m, 1024, std::move(send_msg),
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
                                    std::cout << "websocket upbit api error, file =" << file_name << ", ec = " << ec << ", err msg : " << err_msg << std::endl;
                                    is = 0;
                                    return false;
                                }
                                return true;
                            });

    boost::asio::steady_timer timer{service, std::chrono::steady_clock::now()+ std::chrono::minutes(1)+std::chrono::seconds(1)};
    timer.async_wait([&upbit_api, handler](const auto &ec){
        upbit_api.ws_all_async_close(handler);
    });

    service.run();
    ASSERT_TRUE(is);
}
