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
            return res;
        }

        void insert_tick_data(const std::size_t bars_len,
                              std::vector<std::size_t>& open_times,
                              std::vector<double_type>& opens,
                              std::vector<double_type>& highs,
                              std::vector<double_type>& lows,
                              std::vector<double_type>& closes,
                              std::vector<double_type>& volumes){

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
                }
                open_times.push_back(E);
                opens.push_back(o);
                lows.push_back(l);
                closes.push_back(c);
                highs.push_back(h);
                volumes.push_back(v);
                return;
            }
            std::size_t index = (len <= 0) ? 0 : len-1;
            std::cout << "index -->" << index << std::endl;
            std::cout << "E ->" << E << std::endl;
            std::cout << "o ->" << o << std::endl;
            std::cout << "l ->" << l << std::endl;
            std::cout << "c ->" << c << std::endl;
            std::cout << "h ->" << h << std::endl;
            std::cout << "v ->" << v << std::endl;

            /**
            open_times[index] = E;
            opens[index] = o;
            lows[index] = l;
            closes[index] = c;
            highs[index] = h;
            volumes[index] = v;
             **/
        }

        //friend std::ostream& operator<<(std::ostream &os, const kline_t &o);
    };
}

/***************************************************************************************/

#endif //MAIN_CPP_EXCHANGETYPE_H
