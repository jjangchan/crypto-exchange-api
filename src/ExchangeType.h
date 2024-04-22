//
// Created by jjangchan on 2024/04/01.
//

#ifndef MAIN_CPP_EXCHANGETYPE_H
#define MAIN_CPP_EXCHANGETYPE_H

#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "flatjson.hpp"

#define __STRINGIZE_I(x) #x
#define __STRINGIZE(x) __STRINGIZE_I(x)

#define GMT 9

// file name ( line )
#define __MAKE_FILELINE \
    __FILE__ "(" __STRINGIZE(__LINE__) ")"


// ===================================================================================
// =                                double type                                      =

using double_type = boost::multiprecision::number<
        boost::multiprecision::cpp_dec_float<8>,
        boost::multiprecision::et_off
>;
// ===================================================================================


// ===================================================================================
// =                                json parse                                       =
template<typename T>
typename std::enable_if<std::is_integral<T>::value>::type
__get_json(T &v, const char *member, const flatjson::fjson &j) {
    v = j.at(member).to<T>();
}

template<typename T>
typename std::enable_if<std::is_same<T, std::string>::value>::type
__get_json(T &v, const char *member, const flatjson::fjson &j) {
    const auto &o = j.at(member);
    v = (o.is_null() ? std::string{} : o.to_string());
}

template<typename T>
typename std::enable_if<std::is_same<T, double_type>::value>::type
__get_json(T &v, const char *member, const flatjson::fjson &j) {
    v.assign(j.at(member).to_string());
}


/**
 * obj -> 파싱 되는 구조체
 * #member -> lvalue name
 * json -> json class
 */

#define __JSON_PARSE2(obj, member, json) \
    __get_json(obj.member, #member, json)

#define __JSON_PARSE(res, member, json) __JSON_PARSE2(res, member, json)


// ===================================================================================



// ===================================================================================
// =                                hash fnv1a function                              =

template<typename charPtr>
constexpr std::size_t str_len(const charPtr *str) {
    const charPtr* temp = str;
    while(*str) ++str;
    return str-temp;
}


template<typename charPtr>
constexpr std::uint32_t fnv1a(const charPtr* str, std::size_t len){
    std::uint32_t hash = 0x811c9dc5;
    std::uint32_t prime = 0x01000193;
    for(; len; --len, ++str){
        hash = static_cast<std::uint32_t>(
                ((hash^static_cast<std::uint32_t>(*str))*prime)
        );
    }
    return hash;
}

template<typename charPtr,
        typename = typename std::enable_if<std::is_same<charPtr, const char*>::value>::type>
constexpr std::uint32_t fnv1a(charPtr str){
    return fnv1a(str, str_len(str));
};

template<typename charPtr, std::size_t N>
constexpr std::uint32_t fnv1a(const charPtr (&str)[N]){
    return fnv1a(str, N-1);
}
// ===================================================================================



// ===================================================================================
// =                                timestamp to int                                 =
enum class time_frame : int{
    // mins
    _1m = 1*60,
    _3m = 3*60,
    _5m = 5*60,
    _15m = 15*60,
    _30m = 30*60,

    // hours
    _1h = 1*60*60,
    _2h = 2*60*60,
    _4h = 4*60*60,
    _6h = 6*60*60,
    _8h = 8*60*60,
    _12h = 12*60*60,

    // other
    _1d = 24*60*60,
    _3d = 24*60*60*3,
    _1w = 24*60*60*7,
    _1M = 0
};

const char* time_frame_to_string(time_frame tf){
    switch(tf){
        case time_frame::_1m : return "1m";
        case time_frame::_3m : return "3m";
        case time_frame::_5m : return "5m";
        case time_frame::_15m : return "15m";
        case time_frame::_30m : return "30m";
        case time_frame::_1h : return "1h";
        case time_frame::_2h : return "2h";
        case time_frame::_4h : return "4h";
        case time_frame::_6h : return "6h";
        case time_frame::_8h : return "8h";
        case time_frame::_12h : return "12h";
        case time_frame::_1d : return "1d";
        case time_frame::_3d : return "3d";
        case time_frame::_1w : return "1w";
        case time_frame::_1M : return "1M";
    }
    return "1d";
}


// ===================================================================================


namespace binance{
    static const char* time_frames(const char* time){
        const std::uint32_t hash = fnv1a(time);
        switch (hash) {
            // mins
            case fnv1a("1m"): return "kline_1m";
            case fnv1a("3m"): return "kline_3m";
            case fnv1a("5m"): return "kline_5m";
            case fnv1a("15m"): return "kline_15m";
            case fnv1a("30m"): return "kline_30m";
                // hours
            case fnv1a("1h"): return "kline_1h";
            case fnv1a("2h"): return "kline_2h";
            case fnv1a("4h"): return "kline_4h";
            case fnv1a("6h"): return "kline_6h";
            case fnv1a("8h"): return "kline_8h";
            case fnv1a("12h"): return "kline_12h";
                // days
            case fnv1a("1d"): return "kline_1d";
            case fnv1a("3d"): return "kline_3d";
                // other
            case fnv1a("1w"): return "kline_1w";
            case fnv1a("1M"): return "kline_1M";
                //
            default: return nullptr;
        }
    }

/*************************************************************************************************/
// https://github.com/binance/binance-spot-api-docs/blob/master/web-socket-streams.md#klinecandlestick-streams
    struct kline_t {
        std::size_t E; // Event time
        std::string s; // Symbol
        std::size_t t; // Kline start time
        std::size_t T; // Kline close time
        std::string i; // Interval
        std::size_t f; // First trade ID
        std::size_t L; // Last trade ID
        double_type o; // Open price
        double_type c; // Close price
        double_type h; // High price
        double_type l; // Low price
        double_type v; // Base asset volume
        std::size_t n; // Number of trades
        bool        x; // Is this kline closed?
        double_type q; // Quote asset volume
        double_type V; // Taker buy base asset volume
        double_type Q; // Taker buy quote asset volume

        static kline_t construct(const flatjson::fjson &json){
            assert(json.is_valid());

            kline_t res{};
            __JSON_PARSE(res, E, json);
            __JSON_PARSE(res, s, json);

            const auto k_key = json.at("k");
            // ---> candle data parse
            __JSON_PARSE(res, o, k_key);
            __JSON_PARSE(res, c, k_key);
            __JSON_PARSE(res, h, k_key);
            __JSON_PARSE(res, l, k_key);
            __JSON_PARSE(res, v, k_key);
            __JSON_PARSE(res, x, k_key);
            __JSON_PARSE(res, t, k_key);
            return res;
        }

        void insert_tick_data(const std::size_t bars_len,
                              const time_frame period,
                              std::vector<std::size_t>& open_times,
                              std::vector<double_type>& opens,
                              std::vector<double_type>& highs,
                              std::vector<double_type>& lows,
                              std::vector<double_type>& closes,
                              std::vector<double_type>& volumes){

            if(open_times.empty()){
                open_times.push_back(t);
                opens.push_back(o);
                lows.push_back(l);
                closes.push_back(c);
                highs.push_back(h);
                volumes.push_back(v);
                return;
            }
            std::size_t len = open_times.size();
            if(x){ // ---> new candle
                if(len >= bars_len){
                    for(std::size_t index = 0; index < len-1; index++){
                        open_times[index] = open_times[index+1];
                        opens[index] = opens[index+1];
                        lows[index] = lows[index+1];
                        closes[index] = closes[index+1];
                        highs[index] = highs[index+1];
                        volumes[index] = volumes[index+1];
                    }
                    open_times.pop_back();
                    opens.pop_back();
                    lows.pop_back();
                    closes.pop_back();
                    highs.pop_back();
                    volumes.pop_back();
                }
                open_times.push_back(t);
                opens.push_back(o);
                lows.push_back(l);
                closes.push_back(c);
                highs.push_back(h);
                volumes.push_back(v);
                return;
            }
            open_times[len-1] = t;
            opens[len-1] = o;
            lows[len-1] = l;
            closes[len-1] = c;
            highs[len-1] = h;
            volumes[len-1] = v;
        }

        friend std::ostream& operator<<(std::ostream &os, const kline_t &o);
    };

    std::ostream &operator<<(std::ostream &os, const kline_t &o) {
        os <<
        "E -- > " << o.E << "\n" <<
        "o -- > " << o.o << "\n" <<
        "h -- > " << o.h << "\n" <<
        "l -- > " << o.l << "\n" <<
        "c -- > " << o.c << "\n" <<
        "==============================================================";
        return os;
    }
}


/***************************************************************************************/
namespace bithumb {
    static std::size_t time_stamp(const time_frame period){
        switch (period) {
            case time_frame::_1m : return 100;
            case time_frame::_3m : return 300;
            case time_frame::_5m : return 500;
            case time_frame::_15m : return 1500;
            case time_frame::_30m : return 3000;
            case time_frame::_1h : return 10000;
            case time_frame::_2h : return 20000;
            case time_frame::_4h : return 40000;
            case time_frame::_6h : return 60000;
            case time_frame::_8h : return 80000;
            case time_frame::_12h : return 120000;
            case time_frame::_1d : return 1000000;
            case time_frame::_3d : return 3000000;

        }
        return 100000;
    }
// https://apidocs.bithumb.com/reference/%EB%B9%97%EC%8D%B8-%EA%B1%B0%EB%9E%98%EC%86%8C-%EC%A0%95%EB%B3%B4-%EC%88%98%EC%8B%A0
    struct ticker {
        std::string type;             // 구독 메시지 종류
        std::string symbol;           // 통화코드
        std::string tickType;         // 변동 기준 시간
        std::string date;             // 일자
        std::string time;             // 시간
        double_type openPrice;        // 시가
        double_type closePrice;       // 종가
        double_type lowPrice;         // 저가
        double_type highPrice;        // 고가
        double_type value;            // 누적 거래 금액
        double_type volume;           // 누적 거래량
        double_type sellVolume;       // 매도 누적 거래량
        double_type buyVolume;        // 매수 누적 거래량
        double_type prevClosePrice;   // 전일 종가
        double_type chgRate;          // 변동률
        double_type chgAmt;           // 변동 금액
        double_type volumePower;      // 체결 강도
        std::size_t date_time;


        static ticker construct(const flatjson::fjson &json) {
            assert(json.is_valid());

            ticker res{};
            const auto k_key = json.at("content");
            __JSON_PARSE(res, symbol, k_key);
            __JSON_PARSE(res, tickType, k_key);
            __JSON_PARSE(res, date, k_key);
            __JSON_PARSE(res, time, k_key);
            res.date_time = std::stoul(res.date+res.time);
            // ---> candle data parse
            __JSON_PARSE(res, openPrice, k_key);
            __JSON_PARSE(res, closePrice, k_key);
            __JSON_PARSE(res, lowPrice, k_key);
            __JSON_PARSE(res, highPrice, k_key);
            __JSON_PARSE(res, volume, k_key);
            return res;
        }

        void insert_tick_data(const std::size_t bars_len,
                              const time_frame period,
                              std::vector<std::size_t> &open_times,
                              std::vector<double_type> &opens,
                              std::vector<double_type> &highs,
                              std::vector<double_type> &lows,
                              std::vector<double_type> &closes,
                              std::vector<double_type> &volumes) {

            if (open_times.empty()) {
                open_times.push_back(date_time);
                opens.push_back(openPrice);
                lows.push_back(lowPrice);
                closes.push_back(closePrice);
                highs.push_back(highPrice);
                volumes.push_back(volume);
                return;
            }

            std::size_t len = open_times.size();
            if(period == time_frame::_3d || period == time_frame::_1w || period == time_frame::_1M){

            }else{
                /**
                if (time%interval) { // ---> new candle
                    if (len >= bars_len) {
                        for (std::size_t index = 0; index < len - 1; index++) {
                            open_times[index] = open_times[index + 1];
                            opens[index] = opens[index + 1];
                            lows[index] = lows[index + 1];
                            closes[index] = closes[index + 1];
                            highs[index] = highs[index + 1];
                            volumes[index] = volumes[index + 1];
                        }
                    }
                    open_times.push_back(date_time);
                    opens.push_back(openPrice);
                    lows.push_back(lowPrice);
                    closes.push_back(closePrice);
                    highs.push_back(highPrice);
                    volumes.push_back(volume);
                    return;
                }
                 **/
            }

            open_times[len - 1] = date_time;
            opens[len - 1] = openPrice;
            lows[len - 1] = lowPrice;
            closes[len - 1] = closePrice;
            highs[len - 1] = highPrice;
            volumes[len - 1] = volume;
        }

        //friend std::ostream &operator<<(std::ostream &os, const kline_t &o);
    };

    /**
    std::ostream &operator<<(std::ostream &os, const kline_t &o) {
        os <<
           "E -- > " << o.E << "\n" <<
           "o -- > " << o.o << "\n" <<
           "h -- > " << o.h << "\n" <<
           "l -- > " << o.l << "\n" <<
           "c -- > " << o.c << "\n" <<
           "==============================================================";
        return os;
    }
     **/
}

#endif //MAIN_CPP_EXCHANGETYPE_H
