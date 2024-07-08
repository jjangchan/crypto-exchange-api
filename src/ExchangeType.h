//
// Created by jjangchan on 2024/04/01.
//

#ifndef MAIN_CPP_EXCHANGETYPE_H
#define MAIN_CPP_EXCHANGETYPE_H

#include <string>
#include <iostream>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "uuid/uuid.h"
#include "flatjson.hpp"

<<<<<<< HEAD
#define __STRINGIZE_I(x) #x
#define __STRINGIZE(x) __STRINGIZE_I(x)

#define GMT 9

// file name ( line )
#define __MAKE_FILELINE \
    __FILE__ "(" __STRINGIZE(__LINE__) ")"
=======
#define KST 9
#define GMT 0
>>>>>>> 5651c9286bb7a0bfecb1bfb9bb6bf3fa3a6820a5

/**********************************************************************************************************************/
/*                                             uuid                                                                   */
std::string get_uuid(){
    char uuid[38];
    uuid_t uuidGenerated;
    uuid_generate_random(uuidGenerated);
    uuid_unparse(uuidGenerated, uuid);
    return uuid;
}
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/*                                             double type                                                            */

using double_type = boost::multiprecision::number<
        boost::multiprecision::cpp_dec_float<8>,
        boost::multiprecision::et_off
>;
/**********************************************************************************************************************/


/**********************************************************************************************************************/
/*                                               json parse                                                           */
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
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/*                                       hash fnv1a function                                                          */

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
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/*                                             timestamp                                                              */

struct unix_time_data{
    std::size_t year_start_unix;
    std::size_t year_end_unix;

    std::size_t month_start_unix;
    std::size_t month_end_unix;

    std::size_t week_start_unix;
    std::size_t week_end_unix;

    std::size_t year;
    std::size_t month;
    std::size_t week;

    int sum_months[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

    std::size_t period;

    unix_time_data(const std::size_t _period, const std::size_t local_time = KST) : period(_period){
        std::istringstream iss(current_time());
        int day;

        iss >> month >> day >> year >> week;

        month--, week--, day--;
        year_start_unix = get_gmt_unix_time(year, 0, 1)-(60*60*local_time);

        int wight = 0;
        if((year%4 == 0 && year%100 != 0) || year%400 == 0){
            wight = 1;
            for(int i = 2; i < 13; i++) sum_months[i]++;
        }
        year_end_unix = year_start_unix+(60*60*24*(wight+365));

        month_start_unix = year_start_unix+(sum_months[month] * 60 * 60 * 24);
        month_end_unix = year_start_unix+(sum_months[month+1] * 60 * 60 * 24);

        std::size_t current_unix = month_start_unix+(60*60*24*day);
        week_start_unix = current_unix-(week * 60 * 60 * 24);
        week_end_unix = week_start_unix + (7 * 60 * 60 * 24);

        std::cout << "start year -> " << year_start_unix << std::endl;
        std::cout << "end year -> " << year_end_unix << std::endl;

        std::cout << "start month -> " << month_start_unix << std::endl;
        std::cout << "end month -> " << month_end_unix << std::endl;

        std::cout << "start week -> " << week_start_unix << std::endl;
        std::cout << "end week -> " << week_end_unix << std::endl;
    }

    unix_time_data(){}

    unix_time_data(unix_time_data&& data) noexcept = default;
    unix_time_data& operator=(unix_time_data&& data) noexcept = default;
    unix_time_data(const unix_time_data& data) = default;

    bool is_closed(const std::size_t unix_time){
        bool is = false;
        if(unix_time >= year_end_unix){
            year_start_unix = year_end_unix;
            year++;
            int wight = 0;
            if((year%4 == 0 && year%100 != 0) || year%400 == 0){
                wight = 1;
                for(int i = 2; i < 13; i++) sum_months[i]++;
            }
            year_end_unix = year_start_unix+(60*60*24*(wight+365));
        }
        if(unix_time >= month_end_unix){
            month_start_unix = month_end_unix;
            month = (month+1)%12;
            month_end_unix = year_start_unix+(sum_months[month+1] * 60 * 60 * 24);
            if(period == 0)
                is = true;
        }
        if(unix_time >= week_end_unix){
            week_start_unix = week_end_unix;
            week_end_unix = week_start_unix + (7 * 60 * 60 * 24);
            if(period == (60*60*24*7))
                is = true;
        }
        return is;
    }

    std::string current_time() {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        char buffer[128];
        strftime(buffer, sizeof(buffer), "%m %d %Y %u", now);
        return buffer;
    }

    static size_t get_gmt_unix_time(const std::size_t year,
                                    const std::size_t month,
                                    const std::size_t day,
                                    const std::size_t hour = 0,
                                    const std::size_t min = 0,
                                    const std::size_t sec = 0){
        time_t rawtime;
        struct tm * timeinfo;

        /* get current timeinfo: */
        time ( &rawtime ); //or: rawtime = time(0);
        /* convert to struct: */
        timeinfo = localtime ( &rawtime );

        /* now modify the timeinfo to the given date: */
        timeinfo->tm_year   = year - 1900;
        timeinfo->tm_mon    = month;               //months since January - [0,11]
        timeinfo->tm_mday   = day;                 //day of the month - [1,31]
        timeinfo->tm_hour   = hour;                //hours since midnight - [0,23]
        timeinfo->tm_min    = min;                 //minutes after the hour - [0,59]
        timeinfo->tm_sec    = sec;                 //seconds after the minute - [0,59]

<<<<<<< HEAD


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


=======
        /* call mktime: create unix time stamp from timeinfo struct */
        return timegm( timeinfo );
    }
};

/**********************************************************************************************************************/
>>>>>>> 5651c9286bb7a0bfecb1bfb9bb6bf3fa3a6820a5
namespace binance{
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
        _1w = 24*60*60*7,
        _1M = 0
    };

    static const char* time_frame_to_string(time_frame tf){
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
            case time_frame::_1w : return "1w";
            case time_frame::_1M : return "1M";
        }
        return "1d";
    }

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
                // other
            case fnv1a("1w"): return "kline_1w";
            case fnv1a("1M"): return "kline_1M";
                //
            default: return nullptr;
        }
    }

/**********************************************************************************************************************/
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
                              std::vector<double_type>& volumes,
                              unix_time_data& td){

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

                len = open_times.size();
                std::size_t b_len = len >= bars_len ? len-2 : len-1;
                std::cout <<
                          "t -- > " << (open_times[b_len]/1000) << "\n" <<
                          "o -- > " << opens[b_len] << "\n" <<
                          "h -- > " << highs[b_len] << "\n" <<
                          "l -- > " << lows[b_len] << "\n" <<
                          "c -- > " << closes[b_len] << "\n" <<
                          "==============================================================" << std::endl;
                return;
            }
<<<<<<< HEAD
            open_times[len-1] = t;
=======
>>>>>>> 5651c9286bb7a0bfecb1bfb9bb6bf3fa3a6820a5
            opens[len-1] = o;
            lows[len-1] = l;
            closes[len-1] = c;
            highs[len-1] = h;
            volumes[len-1] = v;
        }

        friend std::ostream& operator<<(std::ostream &os, const kline_t &o);
<<<<<<< HEAD
    };

    std::ostream &operator<<(std::ostream &os, const kline_t &o) {
        os <<
        "E -- > " << o.E << "\n" <<
        "o -- > " << o.o << "\n" <<
        "h -- > " << o.h << "\n" <<
        "l -- > " << o.l << "\n" <<
        "c -- > " << o.c << "\n" <<
        "==============================================================";
=======
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

/**********************************************************************************************************************/
// https://github.com/binance/binance-spot-api-docs/blob/master/rest-api.md#klinecandlestick-data
    struct klines_t {
        struct kline_t {
            std::size_t start_time;
            std::size_t end_time;
            double_type open;
            double_type high;
            double_type low;
            double_type close;
            double_type volume;
            double_type quote_volume;
            std::size_t num_trades;
            double_type taker_buy_base_vol;
            double_type taker_buy_quote_vol;
        };

        std::vector<kline_t> klines;

        static klines_t construct(const flatjson::fjson &json){
            assert(json.is_valid());

            klines_t res{};
            for(std::size_t i = 0u; i < json.size(); ++i){
                kline_t item{};
                const auto it = json.at(i);
                assert(it.is_array());
                item.start_time = it.at(0).to<std::size_t>(); // Open time
                item.open.assign(it.at(1).to_string()); // Open
                item.high.assign(it.at(2).to_string()); // High
                item.low.assign(it.at(3).to_string()); // Low
                item.close.assign(it.at(4).to_string()); // Close
                item.volume.assign(it.at(5).to_string()); // Volume
                item.end_time = it.at(6).to<std::size_t>(); // Close time
                item.quote_volume.assign(it.at(7).to_string()); // Quote asset volume
                item.num_trades = it.at(8).to<std::size_t>(); // Number of trades
                item.taker_buy_base_vol.assign(it.at(9).to_string()); // Taker buy base asset volume
                item.taker_buy_quote_vol.assign(it.at(10).to_string()); // Taker buy quote asset volume

                res.klines.push_back(std::move(item));
            }

            return res;
        }

        void init_tick_data(const std::size_t bars_len,
                            std::vector<std::size_t>& open_times,
                            std::vector<double_type>& opens,
                            std::vector<double_type>& highs,
                            std::vector<double_type>& lows,
                            std::vector<double_type>& closes,
                            std::vector<double_type>& volumes){

            std::size_t index = klines.size() > bars_len ? klines.size()-bars_len : 0;

            for(; index < klines.size(); ++index){
                open_times.push_back(klines[index].start_time);
                opens.push_back(klines[index].open);
                highs.push_back(klines[index].high);
                lows.push_back(klines[index].low);
                closes.push_back(klines[index].close);
                volumes.push_back(klines[index].volume);
            }
        }

        friend std::ostream &operator<<(std::ostream &os, const klines_t &s);
    };

    std::ostream& operator<<(std::ostream &os, const klines_t &s){
        os << "bar size -> " << s.klines.size() << "\n";
        for(int i = 0; i < s.klines.size(); ++i){
            os <<
            "t [" << i << "] -> " << s.klines[i].start_time<< "\n" <<
            "o [" << i << "] -> " << s.klines[i].open << "\n" <<
            "h [" << i << "] -> " << s.klines[i].high << "\n" <<
            "l [" << i << "] -> " << s.klines[i].low << "\n" <<
            "c [" << i << "] -> " << s.klines[i].close << "\n" <<
            "v [" << i << "] -> " << s.klines[i].volume << "\n" <<
            "==============================================" << "\n";
        }
>>>>>>> 5651c9286bb7a0bfecb1bfb9bb6bf3fa3a6820a5
        return os;
    }
}


<<<<<<< HEAD
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
=======
/**********************************************************************************************************************/
namespace bithumb {
    enum class time_frame : int{
        // mins
        _1m = 1*60,
        _3m = 3*60,
        _5m = 5*60,
        _10m = 10*60,
        _30m = 30*60,

        // hours
        _1h = 1*60*60,
        _6h = 6*60*60,
        _12h = 12*60*60,
        _24h = 24*60*60,

        // other
        _1w = 24*60*60*7,
        _1M = 0
    };

    static const char* time_frame_to_string(time_frame tf){
        switch(tf){
            case time_frame::_1m : return "1m";
            case time_frame::_3m : return "3m";
            case time_frame::_5m : return "5m";
            case time_frame::_10m : return "10m";
            case time_frame::_30m : return "30m";
            case time_frame::_1h : return "1h";
            case time_frame::_6h : return "6h";
            case time_frame::_12h : return "12h";
            case time_frame::_24h : return "24h";
            case time_frame::_1w : return "1w";
            case time_frame::_1M : return "1M";
        }
        return "1d";
    }

/**********************************************************************************************************************/

>>>>>>> 5651c9286bb7a0bfecb1bfb9bb6bf3fa3a6820a5
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
<<<<<<< HEAD
                              const time_frame period,
=======
>>>>>>> 5651c9286bb7a0bfecb1bfb9bb6bf3fa3a6820a5
                              std::vector<std::size_t> &open_times,
                              std::vector<double_type> &opens,
                              std::vector<double_type> &highs,
                              std::vector<double_type> &lows,
                              std::vector<double_type> &closes,
<<<<<<< HEAD
                              std::vector<double_type> &volumes) {

            if (open_times.empty()) {
                open_times.push_back(date_time);
                opens.push_back(openPrice);
                lows.push_back(lowPrice);
                closes.push_back(closePrice);
                highs.push_back(highPrice);
=======
                              std::vector<double_type> &volumes,
                              unix_time_data& td) {

            std::size_t len = open_times.size();
            std::size_t unix_time, kline_time = 0;

            std::size_t i_date = std::stoul(date);

            std::size_t y = i_date/10000, M = (i_date%10000)/100, d = i_date%100;
            bool x = false;

            if(td.period == 0){
                unix_time = unix_time_data::get_gmt_unix_time(y, M-1, d)-(KST*60*60);
                kline_time = td.month_start_unix;
                x = td.is_closed(unix_time);
                if(x){
                    kline_time = td.month_start_unix;
                }
            }else if(td.period == (60*60*24*7)){
                unix_time = unix_time_data::get_gmt_unix_time(y, M-1, d)-(KST*60*60);
                kline_time = td.week_start_unix;
                x = td.is_closed(unix_time);
                if(x){
                    kline_time = td.week_start_unix;
                }
            }else{
                std::size_t t_data = std::stoul(time);
                std::size_t h = t_data/10000, m = (t_data%10000)/100, s = t_data%100;
                unix_time = unix_time_data::get_gmt_unix_time(y, M-1, d, h, m, s) - (KST*60*60);
                std::size_t interval = static_cast<std::underlying_type<time_frame>::type>(td.period);
                //std::cout << "unix time ->" << unix_time << std::endl;
                //std::cout << (unix_time/interval)*interval << std::endl;
                kline_time = (unix_time/interval)*interval;
                if(!open_times.empty()){
                    std::size_t kline_end = (open_times[len-1]/1000)+interval;
                    if(unix_time >= kline_end){
                        x = true;
                        kline_time = kline_end;
                    }
                }
            }

            kline_time *= 1000;

            if (open_times.empty()) {
                open_times.push_back(kline_time);
                opens.push_back(openPrice);
                lows.push_back(openPrice);
                closes.push_back(openPrice);
                highs.push_back(openPrice);
>>>>>>> 5651c9286bb7a0bfecb1bfb9bb6bf3fa3a6820a5
                volumes.push_back(volume);
                return;
            }

<<<<<<< HEAD
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
=======
            if (x) { // ---> new candle
                if(kline_time == (unix_time*1000)){
                    double_type prev_close = closes[bars_len-1];
                    if(prev_close == closePrice) return;
                }

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

                open_times.push_back(kline_time);
                opens.push_back(closePrice);
                lows.push_back(closePrice);
                highs.push_back(closePrice);
                closes.push_back(closePrice);
                volumes.push_back(volume);

                len = open_times.size();
                std::size_t b_len = len >= bars_len ? len-2 : len-1;
                std::cout <<
                    "T -- > " << unix_time << "\n" <<
                    "t -- > " << (open_times[b_len]/1000) << "\n" <<
                    "o -- > " << opens[b_len] << "\n" <<
                    "h -- > " << highs[b_len] << "\n" <<
                    "l -- > " << lows[b_len] << "\n" <<
                    "c -- > " << closes[b_len] << "\n" <<
                    "==============================================================" << std::endl;
                return;
            }

            if(closePrice >= highs[len-1]){
                highs[len-1] = closePrice;
            }

            if(closePrice <= lows[len-1]){
                lows[len-1] = closePrice;
            }
            closes[len - 1] = closePrice;
            volumes[len - 1] = volume;
        }

        friend std::ostream &operator<<(std::ostream &os, const ticker &o);
    };

    std::ostream &operator<<(std::ostream &os, const ticker &o) {
        os <<
           "o -- > " << o.openPrice << "\n" <<
           "h -- > " << o.highPrice << "\n" <<
           "l -- > " << o.lowPrice << "\n" <<
           "c -- > " << o.closePrice << "\n" <<
           "==============================================================";
        return os;
    }

/**********************************************************************************************************************/
//https://apidocs.bithumb.com/reference/candlestick-rest-api

    struct candlesticks_t{
        size_t status;
        struct candlestick_t{
            size_t start_time;
            double_type open;
            double_type high;
            double_type low;
            double_type close;
            double_type volume;
        };
        std::vector<candlestick_t> candlesticks;

        static candlesticks_t construct(const flatjson::fjson& json){
            assert(json.is_valid());

            candlesticks_t res{};
            auto data = json.at("data");

            for(std::size_t i = 0u; i < data.size(); ++i){
                candlestick_t item{};
                const auto it = data.at(i);
                assert(it.is_array());
                item.start_time = it.at(0).to<std::size_t>(); // Open time
                item.open.assign(it.at(1).to_string()); // Open
                item.close.assign(it.at(2).to_string()); // Close
                item.high.assign(it.at(3).to_string()); // High
                item.low.assign(it.at(4).to_string()); // Low
                item.volume.assign(it.at(5).to_string()); // Volume
                res.candlesticks.push_back(std::move(item));
            }
            return res;
        }

        void init_tick_data(const std::size_t bars_len,
                            std::vector<std::size_t>& open_times,
                            std::vector<double_type>& opens,
                            std::vector<double_type>& highs,
                            std::vector<double_type>& lows,
                            std::vector<double_type>& closes,
                            std::vector<double_type>& volumes){
            std::size_t index = candlesticks.size() > bars_len ? candlesticks.size()-bars_len : 0;

            for(; index < candlesticks.size(); ++index){
                open_times.push_back(candlesticks[index].start_time);
                opens.push_back(candlesticks[index].open);
                highs.push_back(candlesticks[index].high);
                lows.push_back(candlesticks[index].low);
                closes.push_back(candlesticks[index].close);
                volumes.push_back(candlesticks[index].volume);
            }
        }

        friend std::ostream &operator<<(std::ostream &os, const candlesticks_t& s);
    };

    std::ostream& operator<<(std::ostream &os, const candlesticks_t& s){
        os << "bar size -> " << s.candlesticks.size() << "\n";
        for(int i = 0; i < s.candlesticks.size(); ++i){
            os <<
               "t [" << i << "] -> " << s.candlesticks[i].start_time<< "\n" <<
               "o [" << i << "] -> " << s.candlesticks[i].open << "\n" <<
               "h [" << i << "] -> " << s.candlesticks[i].high << "\n" <<
               "l [" << i << "] -> " << s.candlesticks[i].low << "\n" <<
               "c [" << i << "] -> " << s.candlesticks[i].close << "\n" <<
               "v [" << i << "] -> " << s.candlesticks[i].volume << "\n" <<
               "==============================================" << "\n";
        }
        return os;
    }
}

/**********************************************************************************************************************/

namespace upbit{
    enum class time_frame : int{
        // mins
        _1m = 1*60,
        _3m = 3*60,
        _5m = 5*60,
        _10m = 10*60,
        _15m = 15*60,
        _30m = 30*60,

        // hours
        _1h = 1*60*60,
        _4h = 4*60*60,

        // other
        _1d = 24*60*60,
        _1w = 24*60*60*7,
        _1M = 0
    };

    static const char* time_frame_to_string(time_frame tf){
        switch(tf){
            case time_frame::_1m : return "minutes/1";
            case time_frame::_3m : return "minutes/3";
            case time_frame::_5m : return "minutes/5";
            case time_frame::_10m : return "minutes/10";
            case time_frame::_15m : return "minutes/15";
            case time_frame::_30m : return "minutes/30";
            case time_frame::_1h : return "minutes/60";
            case time_frame::_4h : return "minutes/240";
            case time_frame::_1d : return "days";
            case time_frame::_1w : return "weeks";
            case time_frame::_1M : return "months";
        }
        return "days";
    }

/**********************************************************************************************************************/
//https://docs.upbit.com/v1.4.0/reference/websocket-ticker
    struct ticker{
        std::string type;                      // 타입
        std::string code;                      // 마켓 코드
        double_type opening_price;             // 시가
        double_type high_price;                // 고가
        double_type low_price;                 // 저가
        double_type trade_price;               // 현재가
        double_type prev_closing_price;        // 전일 종가
        std::string change;                    // 전일 대비
        double_type change_price;              // 부호 없는 전일 대비 값
        double_type signed_change_price;       // 전일 대비 값
        double_type change_rate;               // 부호 없는 전일 대비 등략율
        double_type signed_change_rate;        // 전일 대비 등락율
        double_type trade_volume;              // 가장 최근 거래량
        double_type acc_trade_volume;          // 누적 거래량
        double_type acc_trade_volume_24h;      // 24시간 누적 거래량
        double_type acc_trade_price;           // 누적 거래대금
        double_type acc_trade_price_24h;       // 24시간 누적 거래대금
        std::string trade_date;                // 최근 거래 일자(UTC)
        std::string trade_time;                // 최근 거래 시각(UTC)
        std::size_t trade_timestamp;           // 체결 타임스탬프
        std::string ask_bid;                   // 매수/매도 구분
        double_type acc_ask_volume;            // 누적 매도량
        double_type acc_bid_volume;            // 누적 매수량
        double_type highest_52_week_price;     // 52주 최고가
        std::string highest_52_week_date;      // 52주 최고가 달성일
        double_type lowest_52_week_price;      // 52주 최저가
        std::string lowest_52_week_date;       // 52주 최저가 달성일
        std::string trade_status;              // 거래상태*deprecated
        std::string market_state;              // 거래상태
        std::string market_state_for_ios;      // 거래 상태*deprecated
        bool is_trading_suspended;             // 거레 정지 여부
        std::string delisting_date;            // 상장폐지일
        std::string market_warning;            // 유의 종목 여부
        std::size_t timestamp;                 // 타임스탬프
        std::string stream_type;               // 스트림 타입

        static ticker construct(const flatjson::fjson& json){
            assert(json.is_valid());
            ticker res{};
            __JSON_PARSE(res, opening_price, json);
            __JSON_PARSE(res, high_price, json);
            __JSON_PARSE(res, low_price, json);
            __JSON_PARSE(res, trade_price, json);
            __JSON_PARSE(res, trade_volume, json);
            __JSON_PARSE(res, timestamp, json);
            return res;
        }

        void insert_tick_data(const std::size_t bars_len,
                              std::vector<std::size_t> &open_times,
                              std::vector<double_type> &opens,
                              std::vector<double_type> &highs,
                              std::vector<double_type> &lows,
                              std::vector<double_type> &closes,
                              std::vector<double_type> &volumes,
                              unix_time_data& td) {

            std::size_t len = open_times.size();
            std::size_t unix_time = timestamp/1000, kline_time = 0;
            bool x = false;

            if(td.period == 0){
                kline_time = td.month_start_unix;
                x = td.is_closed(unix_time);
                if(x){
                    kline_time = td.month_start_unix;
                }
            }else if(td.period == (60*60*24*7)){
                kline_time = td.week_start_unix;
                x = td.is_closed(unix_time);
                if(x){
                    kline_time = td.week_start_unix;
                }
            }else{
                std::size_t interval = static_cast<std::underlying_type<time_frame>::type>(td.period);
                //std::cout << "unix time ->" << unix_time << std::endl;
                //std::cout << (unix_time/interval)*interval << std::endl;
                kline_time = (unix_time/interval)*interval;
                if(!open_times.empty()){
                    std::size_t kline_end = (open_times[len-1]/1000)+interval;
                    if(unix_time >= kline_end){
                        x = true;
                        kline_time = kline_end;
                    }
                }
            }

            kline_time *= 1000;

            if (open_times.empty()) {
                open_times.push_back(kline_time);
                opens.push_back(trade_price);
                lows.push_back(trade_price);
                closes.push_back(trade_price);
                highs.push_back(trade_price);
                volumes.push_back(trade_volume);
                return;
            }

            if (x) { // ---> new candle

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

                open_times.push_back(kline_time);
                opens.push_back(trade_price);
                lows.push_back(trade_price);
                highs.push_back(trade_price);
                closes.push_back(trade_price);
                volumes.push_back(trade_volume);

                len = open_times.size();
                std::size_t b_len = len >= bars_len ? len-2 : len-1;
                std::cout <<
                          "T -- > " << unix_time << "\n" <<
                          "t -- > " << (open_times[b_len]/1000) << "\n" <<
                          "o -- > " << opens[b_len] << "\n" <<
                          "h -- > " << highs[b_len] << "\n" <<
                          "l -- > " << lows[b_len] << "\n" <<
                          "c -- > " << closes[b_len] << "\n" <<
                          "v -- > " << volumes[b_len] << "\n" <<
                          "==============================================================" << std::endl;
                return;
            }
            if(trade_price >= highs[len-1]){
                highs[len-1] = trade_price;
            }

            if(trade_price <= lows[len-1]){
                lows[len-1] = trade_price;
            }
            closes[len - 1] = trade_price;
            volumes[len - 1] += trade_volume;
        }

    };

/**********************************************************************************************************************/
//https://docs.upbit.com/v1.4.0/reference/%EB%B6%84minute-%EC%BA%94%EB%93%A4-1
    struct candles_t{
        struct candle_t{
            std::string market;
            std::string candle_date_time_utc;
            std::string candle_date_time_kst;
            double_type opening_price;
            double_type high_price;
            double_type low_price;
            double_type trade_price;
            std::size_t timestamp;
            double_type candle_acc_trade_price;
            double_type candle_acc_trade_volume;
        };

        std::vector<candle_t> candles;
        static candles_t construct(const flatjson::fjson& json){
            assert(json.is_valid());
            candles_t res{};
            for(std::size_t i = 0u; i < json.size(); i++){
                candle_t item{};
                const auto it = json.at(i);
                assert(it.is_object());
                __JSON_PARSE(item, opening_price, it);           // 시가
                __JSON_PARSE(item, high_price, it);              // 고가
                __JSON_PARSE(item, low_price, it);               // 저가
                __JSON_PARSE(item, trade_price, it);             // 종가
                __JSON_PARSE(item, candle_acc_trade_volume, it); // 거래량
                __JSON_PARSE(item, candle_date_time_kst, it);    // 시간
                item.candle_date_time_kst.erase(std::remove_if(item.candle_date_time_kst.begin(), item.candle_date_time_kst.end(),
                                                               [](unsigned char x){return x == '-' || x == ':' || x == 'T';})-2,
                                                item.candle_date_time_kst.end());
                std::size_t time = std::stoul(item.candle_date_time_kst);
                size_t y, M, d, h, m;
                y = (time/100000000), M = (time/1000000)%100, d = (time/10000)%100;
                h = (time/100)%100, m = time%100;

                item.timestamp = (unix_time_data::get_gmt_unix_time(y, M-1, d, h, m, 0) - (KST*60*60))*1000;
                res.candles.push_back(std::move(item));
            }

            return res;
        }

        void init_tick_data(const std::size_t bars_len,
                            std::vector<std::size_t>& open_times,
                            std::vector<double_type>& opens,
                            std::vector<double_type>& highs,
                            std::vector<double_type>& lows,
                            std::vector<double_type>& closes,
                            std::vector<double_type>& volumes){
            std::size_t index = candles.size();
            size_t end = bars_len >= index ? 1 : index-bars_len;
            while(index >= end){
                --index;
                open_times.push_back(candles[index].timestamp);
                opens.push_back(candles[index].opening_price);
                highs.push_back(candles[index].high_price);
                lows.push_back(candles[index].low_price);
                closes.push_back(candles[index].trade_price);
                volumes.push_back(candles[index].candle_acc_trade_volume);
            }
        }

        friend std::ostream &operator<<(std::ostream &os, const candles_t& s);

    };

    std::ostream& operator<<(std::ostream &os, const candles_t& s){
        os << "bar size -> " << s.candles.size() << "\n";
        for(int i = 0; i < s.candles.size(); ++i){
            os <<
               "t [" << i << "] -> " << s.candles[i].timestamp << "\n" <<
               "o [" << i << "] -> " << s.candles[i].opening_price << "\n" <<
               "h [" << i << "] -> " << s.candles[i].high_price << "\n" <<
               "l [" << i << "] -> " << s.candles[i].low_price << "\n" <<
               "c [" << i << "] -> " << s.candles[i].trade_price << "\n" <<
               "v [" << i << "] -> " << s.candles[i].candle_acc_trade_volume << "\n" <<
               "==============================================" << "\n";
        }
        return os;
    }

}

/**********************************************************************************************************************/

namespace coinbase{
    enum class time_frame : int{
        // mins
        _1m = 1*60,
        _5m = 5*60,
        _15m = 15*60,

        // hours
        _1h = 1*60*60,
        _6h = 6*60*60,

        // other
        _1d = 24*60*60,
    };

    static const char* time_frame_to_string(time_frame tf){
        switch(tf){
            case time_frame::_1m : return "60";
            case time_frame::_5m : return "300";
            case time_frame::_15m : return "900";
            case time_frame::_1h : return "3600";
            case time_frame::_6h : return "21600";
            case time_frame::_1d : return "86400";
        }
        return "60";
    }

/**********************************************************************************************************************/
//https://docs.cdp.coinbase.com/advanced-trade/docs/ws-channels/#candles-channel
    struct ticker{
        std::string type;
        std::size_t sequence;
        std::string product_id;
        double_type price;
        double_type last_size;
        std::string time;

        static ticker construct(const flatjson::fjson& json){
            assert(json.is_valid());
            ticker res{};
            // ---> candle data parse
            __JSON_PARSE(res, product_id, json);
            __JSON_PARSE(res, price, json);
            __JSON_PARSE(res, last_size, json);
            __JSON_PARSE(res, time, json);
            res.time.erase((res.time.begin()+res.time.find('.')), res.time.end());
            res.time.erase(std::remove_if(res.time.begin(), res.time.end(),
                                      [](unsigned char x){return x == '-' || x == ':' || x == 'T';}),
                       res.time.end());
            return res;
        }

        void insert_tick_data(const std::size_t bars_len,
                              std::vector<std::size_t> &open_times,
                              std::vector<double_type> &opens,
                              std::vector<double_type> &highs,
                              std::vector<double_type> &lows,
                              std::vector<double_type> &closes,
                              std::vector<double_type> &volumes,
                              unix_time_data& td) {

            std::size_t len = open_times.size();
            std::size_t unix_time, kline_time = 0;
            std::size_t i_time = std::stoul(time);

            size_t y, M, d, h, m, s;
            y = (i_time/10000000000), M = (i_time/100000000)%100, d = (i_time/1000000)%100;
            bool x = false;

            if(td.period == 0) {
                unix_time = unix_time_data::get_gmt_unix_time(y, M - 1, d);
                kline_time = td.month_start_unix;
                x = td.is_closed(unix_time);
                if (x) {
                    kline_time = td.month_start_unix;
                }
            }else{
                h = (i_time/10000)%100, m = (i_time/100)%100, s = i_time%100;
                unix_time = unix_time_data::get_gmt_unix_time(y, M-1, d, h, m, s);
                std::size_t interval = static_cast<std::underlying_type<time_frame>::type>(td.period);
                kline_time = (unix_time/interval)*interval;
                if(!open_times.empty()){
                    std::size_t kline_end = (open_times[len-1]/1000)+interval;
                    if(unix_time >= kline_end){
                        x = true;
                        kline_time = kline_end;
                    }
                }
            }

            kline_time *= 1000;

            if (open_times.empty()) {
                open_times.push_back(kline_time);
                opens.push_back(price);
                lows.push_back(price);
                closes.push_back(price);
                highs.push_back(price);
                volumes.push_back(last_size);
                return;
            }

            if (x) { // ---> new candle
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

                open_times.push_back(kline_time);
                opens.push_back(price);
                lows.push_back(price);
                highs.push_back(price);
                closes.push_back(price);
                volumes.push_back(last_size);

                std::size_t b_len = len >= bars_len ? len-2 : len-1;
                std::cout <<
                          "T -- > " << unix_time << "\n" <<
                          "t -- > " << (open_times[b_len]/1000) << "\n" <<
                          "o -- > " << opens[b_len] << "\n" <<
                          "h -- > " << highs[b_len] << "\n" <<
                          "l -- > " << lows[b_len] << "\n" <<
                          "c -- > " << closes[b_len] << "\n" <<
                          "==============================================================" << std::endl;
                return;
            }

            if(price >= highs[len-1]){
                highs[len-1] = price;
            }

            if(price <= lows[len-1]){
                lows[len-1] = price;
            }
            closes[len - 1] = price;
            volumes[len - 1] += last_size;
        }

    };

/**********************************************************************************************************************/
//https://docs.cdp.coinbase.com/exchange/reference/exchangerestapi_getproductcandles/
    struct candles_t{
        struct candle_t{
            std::size_t timestamp;
            double_type low;
            double_type high;
            double_type open;
            double_type close;
            double_type volume;
        };

        std::vector<candle_t> candles;
        static candles_t construct(const flatjson::fjson& json){
            assert(json.is_valid());
            candles_t res{};
            for(std::size_t i = 0u; i < json.size(); i++){
                candle_t item{};
                const auto it = json.at(i);
                assert(it.is_array());

                item.timestamp = it.at(0).to<std::size_t>();
                item.low.assign(it.at(1).to_string());
                item.high.assign(it.at(2).to_string());
                item.open.assign(it.at(3).to_string());
                item.close.assign(it.at(4).to_string());
                item.volume.assign(it.at(5).to_string());

                res.candles.push_back(std::move(item));
            }

            return res;
        }

        void init_tick_data(const std::size_t bars_len,
                            std::vector<std::size_t>& open_times,
                            std::vector<double_type>& opens,
                            std::vector<double_type>& highs,
                            std::vector<double_type>& lows,
                            std::vector<double_type>& closes,
                            std::vector<double_type>& volumes){
            std::size_t index = candles.size();
            size_t end = bars_len >= index ? 1 : index-bars_len;
            while(index >= end){
                --index;
                open_times.push_back(candles[index].timestamp*1000);
                opens.push_back(candles[index].open);
                highs.push_back(candles[index].high);
                lows.push_back(candles[index].low);
                closes.push_back(candles[index].close);
                volumes.push_back(candles[index].volume);
            }

        }

        friend std::ostream &operator<<(std::ostream &os, const candles_t& s);

    };

    std::ostream& operator<<(std::ostream &os, const candles_t& s){
        os << "bar size -> " << s.candles.size() << "\n";
        for(int i = 0; i < s.candles.size(); ++i){
            os <<
               "t [" << i << "] -> " << s.candles[i].timestamp << "\n" <<
               "o [" << i << "] -> " << s.candles[i].open << "\n" <<
               "h [" << i << "] -> " << s.candles[i].high << "\n" <<
               "l [" << i << "] -> " << s.candles[i].low << "\n" <<
               "c [" << i << "] -> " << s.candles[i].close << "\n" <<
               "v [" << i << "] -> " << s.candles[i].volume << "\n" <<
               "==============================================" << "\n";
        }
        return os;
    }

>>>>>>> 5651c9286bb7a0bfecb1bfb9bb6bf3fa3a6820a5
}

#endif //MAIN_CPP_EXCHANGETYPE_H
