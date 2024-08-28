//
// Created by jjangchan on 2024/08/27.
//

#include <gtest/gtest.h>
#include <iostream>
#include "../src/BinanceAPI.h"

std::string pk;
std::string sk;

int main(int argc, char** argv) {
    if(argc == 3){
        pk = argv[1];
        sk = argv[2];
    }
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


class BinanceOrderTest: public ::testing::Test{
public:
protected:
    BinanceOrderTest() {
    }
    virtual ~BinanceOrderTest(){}

public:
    virtual void SetUp(){
        std::cout << "SET UP BINANCE ORDER" << std::endl;
        if(pk.empty() && sk.empty()) GTEST_SKIP_("!!!!!!!!!!!! not set pk and sk");
    }
    virtual void TearDown(){
        std::cout << "TEAR DOWN BINANCE ORDER" << std::endl;
    }
};

TEST_F(BinanceOrderTest, binance_order_market_send_test){
    boost::asio::io_context service;
    BinanceAPI binance_api(service,
                           "stream.binance.com",
                           "9443",
                           "testnet.binance.vision",
                           "443",
                           pk,
                           sk,
                           10000,
                           "");

    // ---> market buy test
    binance::send_info b_info("LINKUSDT",
                            binance::enum_side::buy,
                            binance::enum_type::market,
                            binance::enum_time::GTC,
                            "1",
                            {}, {}, {}, {}, {}, {});
    auto b_re = binance_api.order_send(b_info);
    if ( !b_re ) {
        std::cerr << "get send error: " << b_re.err_msg << std::endl;
        ASSERT_TRUE(0);
    }
    std::cout << b_re.v << std::endl;

    // ---> market sell test
    binance::send_info s_info("LINKUSDT",
                              binance::enum_side::sell,
                              binance::enum_type::market,
                              binance::enum_time::GTC,
                              "1",
                              {}, {}, {}, {}, {}, {});
    auto s_re = binance_api.order_send(s_info);
    if ( !s_re ) {
        std::cerr << "get send error: " << s_re.err_msg << std::endl;
        ASSERT_TRUE(0);
    }
    std::cout << s_re.v << std::endl;
    ASSERT_TRUE(1);
}

TEST_F(BinanceOrderTest, binance_order_limit_send_test){
    boost::asio::io_context service;
    BinanceAPI binance_api(service,
                           "stream.binance.com",
                           "9443",
                           "testnet.binance.vision",
                           "443",
                           pk,
                           sk,
                           10000,
                           "");

    // ---> limit buy test
    binance::send_info b_info("LINKUSDT",
                              binance::enum_side::buy,
                              binance::enum_type::limit,
                              binance::enum_time::GTC,
                              "1", "10",
                              {}, {}, {}, {}, {});
    auto b_re = binance_api.order_send(b_info);
    if ( !b_re ) {
        std::cerr << "get send error: " << b_re.err_msg << std::endl;
        ASSERT_TRUE(0);
    }
    std::cout << b_re.v << std::endl;
}

TEST_F(BinanceOrderTest, binance_order_all_open_test){
    ASSERT_TRUE(!sk.empty() && !pk.empty());
    boost::asio::io_context service;
    BinanceAPI binance_api(service,
                           "stream.binance.com",
                           "9443",
                           "testnet.binance.vision",
                           "443",
                           pk,
                           sk,
                           10000,
                           "");
    binance::open_info info("LINKUSDT", true);
    auto re = binance_api.order_open(info);
    if ( !re ) {
        std::cerr << "get error: " << re.err_msg << std::endl;
        ASSERT_TRUE(0);
    }
    std::cout << re.v << std::endl;
    ASSERT_TRUE(1);
}


TEST_F(BinanceOrderTest, binance_order_open_test){
    boost::asio::io_context service;
    BinanceAPI binance_api(service,
                           "stream.binance.com",
                           "9443",
                           "testnet.binance.vision",
                           "443",
                           pk,
                           sk,
                           10000,
                           "");
    binance::open_info info("LINKUSDT");
    auto re = binance_api.order_open(info);
    if ( !re ) {
        std::cerr << "get error: " << re.err_msg << std::endl;
        ASSERT_TRUE(0);
    }
    std::cout << re.v << std::endl;
    ASSERT_TRUE(1);
}

TEST_F(BinanceOrderTest, binance_order_cancel_test){
    boost::asio::io_context service;
    BinanceAPI binance_api(service,
                           "stream.binance.com",
                           "9443",
                           "testnet.binance.vision",
                           "443",
                           pk,
                           sk,
                           10000,
                           "");
    auto re = binance_api.order_open({"LINKUSDT"});
    if ( !re ) {
        std::cerr << "get error: " << re.err_msg << std::endl;
        ASSERT_TRUE(0);
    }
    const binance::open_orders* o = static_cast<const binance::open_orders*>(re.v.get_open());
    ASSERT_TRUE(!o->orders.empty());
    const binance::open_order open = o->orders.begin()->second[0];


    binance::cancel_info info(open.symbol.c_str(),
                              open.orderId,
                              open.clientOrderId.c_str());
    auto re2 = binance_api.order_cancel(info);
    if ( !re2 ) {
        std::cerr << "get error: " << re2.err_msg << std::endl;
        ASSERT_TRUE(0);
    }
    std::cout << re2.v << std::endl;
    ASSERT_TRUE(1);
}

TEST_F(BinanceOrderTest, binance_order_all_cancel_test){
    boost::asio::io_context service;
    BinanceAPI binance_api(service,
                           "stream.binance.com",
                           "9443",
                           "testnet.binance.vision",
                           "443",
                           pk,
                           sk,
                           10000,
                           "");

    binance::send_info b1_info("LINKUSDT",
                              binance::enum_side::buy,
                              binance::enum_type::limit,
                              binance::enum_time::GTC,
                              "1", "10",
                              {}, {}, {}, {}, {});
    auto b1_re = binance_api.order_send(b1_info);
    if ( !b1_re ) {
        std::cerr << "get send error: " << b1_re.err_msg << std::endl;
        ASSERT_TRUE(0);
    }

    binance::send_info b2_info("LINKUSDT",
                              binance::enum_side::buy,
                              binance::enum_type::limit,
                              binance::enum_time::GTC,
                              "1", "10.5",
                              {}, {}, {}, {}, {});
    auto b2_re = binance_api.order_send(b2_info);
    if ( !b2_re ) {
        std::cerr << "get send error: " << b2_re.err_msg << std::endl;
        ASSERT_TRUE(0);
    }

    auto c_re = binance_api.order_cancel({"LINKUSDT"});
    if ( !c_re ) {
        std::cerr << "get error: " << c_re.err_msg << std::endl;
        ASSERT_TRUE(0);
    }
    std::cout << c_re.v << std::endl;
    ASSERT_TRUE(1);


}