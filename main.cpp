#include <iostream>
#include <chrono>
#include "src/BithumbAPI.h"
#include "src/BinanceAPI.h"


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

    unix_time_data(){
        std::istringstream iss(current_time());
        int day;

        iss >> month >> day >> year >> week;

        std::cout << "week : " << week << std::endl;

        time_t rawtime;
        struct tm * timeinfo;


        /* get current timeinfo: */
        time ( &rawtime ); //or: rawtime = time(0);
        /* convert to struct: */
        timeinfo = localtime ( &rawtime );

        /* now modify the timeinfo to the given date: */
        timeinfo->tm_year   = year - 1900;
        timeinfo->tm_mon    = 0;               //months since January - [0,11]
        timeinfo->tm_mday   = 1;               //day of the month - [1,31]
        timeinfo->tm_hour   = 0;               //hours since midnight - [0,23]
        timeinfo->tm_min    = 0;               //minutes after the hour - [0,59]
        timeinfo->tm_sec    = 0;               //seconds after the minute - [0,59]

        /* call mktime: create unix time stamp from timeinfo struct */
        year_start_unix = timegm( timeinfo );
        int wight = 0;
        if((year%4 == 0 && year%100 != 0) || year%400 == 0){
            wight = 1;
            for(int i = 2; i < 13; i++) sum_months[i]++;
        }
        year_end_unix = year_start_unix+(60*60*24*(wight+365));

        month--, week--, day--;

        month_start_unix = year_start_unix+(sum_months[month] * 60 * 60 * 24);
        month_end_unix = year_start_unix+(sum_months[month+1] * 60 * 60 * 24);

        std::size_t current_unix = month_start_unix+(60*60*24*day);
        std::cout << current_unix << std::endl;
        week_start_unix = current_unix-(week * 60 * 60 * 24);
        week_end_unix = week_start_unix+(7 * 60 * 60 * 24);

        std::cout << year_start_unix << std::endl;
        std::cout << month_start_unix << std::endl;
        std::cout << month_end_unix << std::endl;
        std::cout << year_end_unix << std::endl;

        std::cout << week_start_unix << std::endl;
        std::cout << week_end_unix << std::endl;
    }

    std::string current_time() {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        char buffer[128];
        strftime(buffer, sizeof(buffer), "%m %d %Y %u", now);
        return buffer;
    }
};

int main() {

    unix_time_data unix_data;

    return 0;

    boost::asio::io_context service;
    std::string send_msg =  BithumbAPI::make_send_msg("ticker", {"BTC_KRW"}, {"30M"});
    BithumbAPI bithumb_api(service, "pubwss.bithumb.com", "443", send_msg, 1024);
    auto handler = bithumb_api.bithumb_on_tick(time_frame::_1m, [](const char* file_name,
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
            std::cerr << "websocket binance api error, file =" << file_name << ", ec = " << ec << ", err msg : " << err_msg << std::endl;
            return false;
        }
        //std::size_t v_size = opens.size();
        //std::cout << "bar size --> " << v_size << std::endl;
        //std::cout << msg << std::endl;
        return true;
    });
     /**

    BinanceAPI binance_api(service,"stream.binance.com", "9443", {}, 1024);
    auto handler = binance_api.binance_on_tick("BTCUSDT", "1m", [](const char* file_name,
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
            std::cerr << "websocket binance api error, file =" << file_name << ", ec = " << ec << ", err msg : " << err_msg << std::endl;
            return false;
        }
        std::size_t v_size = opens.size();
        std::cout << "bar size --> " << v_size << std::endl;
        //std::cout << msg << std::endl;
        return true;
    });
    **/

    service.run();
    return 0;
}
