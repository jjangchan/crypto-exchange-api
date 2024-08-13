//
// Created by jjangchan on 2024/07/12.
//

#include <gtest/gtest.h>
#include <iostream>
#include "../src/CoinBaseAPI.h"

class CoinBaseTest: public ::testing::Test{
public:
protected:
    CoinBaseTest(){}
    virtual ~CoinBaseTest(){}

public:
    virtual void SetUp(){
        std::cout << "SET UP COINBASE ======================================================" << std::endl;
    }
    virtual void TearDown(){
        std::cout << "TEAR DOWN COINBASE ======================================================" << std::endl;
    }
};

TEST_F(CoinBaseTest, coinbase_ws_1m_timer_test){
    boost::asio::io_context service;
    int is = 1;

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
                                                         std::cout << "websocket coinbase api error, file =" << file_name << ", ec = " << ec << ", err msg : " << err_msg << std::endl;
                                                         is = 0;
                                                         return false;
                                                     }
                                                     return true;
                                                 });

    boost::asio::steady_timer timer{service, std::chrono::steady_clock::now()+ std::chrono::minutes(1)+std::chrono::seconds(1)};
    timer.async_wait([&coinbase_api, handler](const auto &ec){
        coinbase_api.ws_all_async_close(handler);
    });

    service.run();
    ASSERT_TRUE(is);
}
