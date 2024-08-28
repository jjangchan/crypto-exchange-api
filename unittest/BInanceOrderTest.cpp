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
        std::cout << "SET UP BINANCE ======================================================" << std::endl;

    }
    virtual void TearDown(){
        std::cout << "TEAR DOWN BINANCE ======================================================" << std::endl;
    }
};

/**
TEST_F(BinanceOrderTest, binance_order_send_test){
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
    binance::send_info info("LINKUSDT",
                            binance::enum_side::buy,
                            binance::enum_type::limit,
                            binance::enum_time::GTC,
                            "1",
                            "11", "100", "1000001",
                            {}, {}, {});
    auto re = binance_api.order_send(info);
    if ( !re ) {
        std::cerr << "get error: " << re.err_msg << std::endl;
        ASSERT_TRUE(0);
    }
    ASSERT_TRUE(1);
}
 **/

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
    binance::open_info info("LINKUSDT");
    auto re = binance_api.order_open(info);
    if ( !re ) {
        std::cerr << "get error: " << re.err_msg << std::endl;
        ASSERT_TRUE(0);
    }
    ASSERT_TRUE(1);
}

TEST_F(BinanceOrderTest, binance_order_open_test){
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
    binance::open_info info("LINKUSDT");
    auto re = binance_api.order_open(info);
    if ( !re ) {
        std::cerr << "get error: " << re.err_msg << std::endl;
        ASSERT_TRUE(0);
    }
    const binance::open_orders* o = static_cast<const binance::open_orders*>(re.v.get_open());

    ASSERT_TRUE(!o->orders.empty());
    binance::open_info info2("LINKUSDT",
                             o->orders.begin()->second[0].orderId,
                             o->orders.begin()->second[0].clientOrderId.c_str());
    auto re2 = binance_api.order_open(info2);
    if ( !re2 ) {
        std::cerr << "get error: " << re2.err_msg << std::endl;
        ASSERT_TRUE(0);
    }
    ASSERT_TRUE(1);
}

