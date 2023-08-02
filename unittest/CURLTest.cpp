//
// Created by jjangchan on 2022/03/31.
//
#include <gtest/gtest.h>
#include <iostream>
#include <curl/curl.h>

class CURLTest: public ::testing::Test{
public:
    CURL* curl;
protected:
    CURLTest() {
    }
    virtual ~CURLTest(){}

public:
    virtual void SetUp(){
        std::cout << "set up" << std::endl;

    }
    virtual void TearDown(){
        std::cout << "tear Down" << std::endl;

    }
};

TEST_F(CURLTest, curlInitTest){
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    ASSERT_TRUE(curl);
}
