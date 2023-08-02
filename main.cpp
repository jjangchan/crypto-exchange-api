#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <curl/curl.h>
#include <string>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>
#define BINANCE_HOST "https://api.binance.com"

// Curl's callback
size_t
callBackFunk( void *content, size_t size, size_t nmemb, std::string *buffer )
{
    buffer->append((char*)content, size*nmemb);
    return size*nmemb;
}

std::string curl_post_url(std::string& url){
    CURL *curl = NULL;
    CURLcode res;
    struct curl_slist *headers=NULL; // init to NULL is important
    curl_global_init(CURL_GLOBAL_ALL); // Initialize everything possible
    curl = curl_easy_init();
    // ---- header setting
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");
    // ----
    std::string chunk;
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callBackFunk);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (std::string*)&chunk);
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    curl_slist_free_all(headers);
    curl_global_cleanup();
    if(res == CURLE_OK) {
        return chunk;
    } else {
        throw std::runtime_error("CURL error, CURLE_CODE (" + std::to_string(res) + ")");
    }

}

int main() {
    std::string url(BINANCE_HOST);
    url += "/api/v1/exchangeInfo";
    auto start = std::chrono::system_clock::now();
    std::string result = curl_post_url(url);
    auto end = std::chrono::system_clock::now();
    auto int64_time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << int64_time << std::endl;
    //std::cout << result << std::endl;
    return 0;
}
