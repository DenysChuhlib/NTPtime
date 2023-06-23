/*

Денис Чухліб ,https://github.com/DenysChuhlib
MIT License

*/

#ifndef TimeFunc_H
#define TimeFunc_H

const PROGMEM uint8_t UNIX_LAST_DAY_OF_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#include "languages/en.h"
#include "languages/ua.h"
#include "languages/ru.h"

#ifdef UNIX64
typedef int64_t unix_t;

typedef int64_t year_t;
#else
typedef uint32_t unix_t;

typedef uint16_t year_t;
#endif

struct TimeFunctions {
    //
    static bool isAM(uint8_t h) {
        return !isPM(h);  
    }
    
    //
    static bool isPM(uint8_t h) {
        return (h >= 12); 
    }
    
    //
    static uint8_t hourFormat12(uint8_t h) {
        if(h == 0 ) return 12;
        else if(h > 12) return h - 12 ;
        else return h ;
    }
    
    static unix_t nextDays(unix_t unix, unix_t n) {
        unix += (unix_t)86400UL * n;
        return unix;
    }
    static unix_t nextDay(unix_t unix) {
        unix += 86400UL;
        return unix;
    }
    
    static unix_t prevDays(unix_t unix, unix_t n) {
        unix -= (unix_t)86400UL * n;
        return unix;
    }
    static unix_t prevDay(unix_t unix) {
        unix -= 86400UL;
        return unix;
    }
    
    //
    static bool everyH(uint8_t time_now, uint8_t time_last, uint8_t time_out) {
        int8_t a = time_now;
        if (time_out == 24 && time_last == a) return true;
        a -= time_last;
        if (a < 0) a = -a;
        if (a >= time_out) return true;
        return false;
    }
    
    //
    static bool everyM(uint8_t time_now, uint8_t time_last, uint8_t time_out) {
        return everyS(time_now, time_last, time_out);
    }
    
    //
    static bool everyS(uint8_t time_now, uint8_t time_last, uint8_t time_out) {
        int8_t a = time_now;
        if (time_out == 60 && time_last == a) return true;
        a -= time_last;
        if (a < 0) a = -a;
        if (a >= time_out) return true;
        return false;
    }
    
    //
    static bool everyMs(uint16_t time_now, uint16_t time_last, uint16_t time_out) {
        int16_t a = time_now;
        if (time_out == 1000 && time_last == a) return true;
        a -= time_last;
        if (a < 0) a = -a;
        if (a >= time_out) return true;
        return false;
    }
    
    //
    static unix_t periodInSec(unix_t unix_now, unix_t last_unix) {
        return unix_now - last_unix;
    }
    
    //
    static unix_t periodInFullDays(unix_t unix_now, unix_t last_unix) {
        return periodInSec(unix_now, last_unix) / 86400UL;
    }
    
    //
    static unix_t periodInDays(unix_t unix_now, unix_t last_unix) {
        return (unix_now / 86400UL) - (last_unix / 86400UL);
    }
    
    //
    static unix_t periodInMonths(unix_t unix_now, unix_t last_unix) {
        year_t year;
        uint8_t month, day;
        decodeUNIX(unix_now, day, month, year);
        year_t year_last;
        uint8_t month_last, day_last;
        decodeUNIX(last_unix, day_last, month_last, year_last);
        
        year_t p;
    
        p = (year - year_last) * 12;
        p += month - month_last;
        if (day < day_last) {
            p--;
        }
        return p;
    }
    
    //========================================================================
    static bool timeOutMonth(unix_t unix_now, unix_t last_unix, unix_t time_out) {
        uint32_t a = periodInMonths(unix_now, last_unix);
        if (a >= time_out) return true;
        return false;
    }
    
    //
    static bool timeOutD(unix_t unix_now, unix_t last_unix, unix_t time_out) {
        uint32_t a = periodInDays(unix_now, last_unix);
        if (a >= time_out) return true;
        return false;
    }
    
    //
    static bool timeOutFullDays(unix_t unix_now, unix_t last_unix, unix_t time_out) {
        uint32_t a = periodInFullDays(unix_now, last_unix);
        if (a >= time_out) return true;
        return false;
    }
    
    //
    static bool timeOutH(unix_t unix_now, unix_t last_unix, unix_t time_out) {
        unix_t a = periodInSec(unix_now, last_unix);
        time_out = time_out * 3600;
        if (a >= time_out) return true;
        return false;
    }
    
    //
    static bool timeOutM(unix_t unix_now, unix_t last_unix, unix_t time_out) {
        unix_t a = periodInSec(unix_now, last_unix);
        time_out = time_out * 60;
        if (a >= time_out) return true;
        return false;
    }
    
    //
    static bool timeOutS(unix_t unix_now, unix_t last_unix, unix_t time_out) {
        unix_t a = periodInSec(unix_now, last_unix);
        if (a >= time_out) return true;
        return false;
    }
    
    static bool isLeap(year_t y) {
        return  y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
    }
    
    static uint8_t lastDayOfMonth(uint8_t m, year_t y) {
        return m != 2 || !isLeap(y) ? pgm_read_byte(&UNIX_LAST_DAY_OF_MONTH[m-1]) : 29u;
    }
    
    static unix_t unixGMTFromDate(year_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM) {
        int8_t my = (m >= 3) ? 1 : 0;
        y += my - 1970;
        uint16_t dm = 0;
        for (int i = 0; i < m - 1; i++) dm += (i<7)?((i==1)?28:((i&1)?30:31)):((i&1)?31:30);
        return (((d-1+dm+((y+1)>>2)-((y+69)/100)+((y+369)/100/4)+365*(y-my))*24ul+h-(tzM/60))*60ul+mn-(tzM%60))*60ul+s;
    }
    
    static void decodeUNIX(unix_t unix, uint8_t& d, uint8_t& m, year_t& y) {
        // http://howardhinnant.github.io/date_algorithms.html#civil_from_days
        unix_t u = unix / 86400ul;
        u += 719468ul;
        unix_t era = (u >= 0 ? u : u - 146096) / 146097;
        uint16_t doe = u - era * 146097ul;
        uint16_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
        y = yoe + era * 400;
        uint16_t doy = doe - (yoe * 365 + yoe / 4 - yoe / 100);
        uint16_t mp = (doy * 5 + 2) / 153;
        d = doy - (mp * 153 + 2) / 5 + 1;
        m = mp + (mp < 10 ? 3 : -9);
        y += (m <= 2);
    }
    
    static void decodeUNIX(unix_t unix, uint8_t& d, uint8_t& m, year_t& y, uint8_t& h, uint8_t& min, uint8_t& s) {
        s = unix % 60;
        min = (unix % 3600UL) / 60UL;
        h = (unix % 86400UL) / 3600UL;
        decodeUNIX(unix, d, m, y);
    }
    
    static uint8_t dayWeek(unix_t unix) {
        uint8_t _dayw = ((unix / 86400UL) + 4) % 7;
        if (!_dayw) _dayw = 7;
        return _dayw;
    }
    
    static void delay(uint32_t t, void (*func)()) {
        uint32_t a = micros();
        while (t > 0) {
            if (*func) func();
            while (t > 0 && (micros() - a) >= 1000) {
                t--;
                a += 1000;
            }
            yield();
        }
    }
    
    static unix_t getUnixGMTCompiled(int16_t tzM = 0) {
        return unixGMTFromDate(
            ((__DATE__[7]-'0')*1000+(__DATE__[8]-'0')*100 + (__DATE__[9]-'0')*10+(__DATE__[10]-'0')),//year
            ((__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')   ? 1 : //month
            (__DATE__[0] == 'F')                                                ? 2 : 
            (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')    ? 3 : 
            (__DATE__[0] == 'A' && __DATE__[1] == 'p')                          ? 4 : 
            (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')    ? 5 : 
            (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')    ? 6 : 
            (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')    ? 7 : 
            (__DATE__[0] == 'A' && __DATE__[1] == 'u')                          ? 8 : 
            (__DATE__[0] == 'S')                                                ? 9 : 
            (__DATE__[0] == 'O')                                                ? 10 :
            (__DATE__[0] == 'N')                                                ? 11 :
            (__DATE__[0] == 'D')                                                ? 12 : 0),//month
            ((((__DATE__[4] >= '0') ? (__DATE__[4]) : '0')-'0')*10+(__DATE__[5]-'0')),//day
            ((__TIME__[0]-'0')*10+(__TIME__[1]-'0')),//hour
            ((__TIME__[3]-'0')*10+(__TIME__[4]-'0')),//minute
            ((__TIME__[6]-'0')*10+(__TIME__[7]-'0')),//second
            tzM);//compiler time zone
    }
    
/*
    | specifier | output                                                |
    |-----------|-------------------------------------------------------|
    | YYYY      | the year as a 4-digit number (2000--2099)             |
    | YY        | the year as a 2-digit number (00--99)                 |
    | MM        | the month as a 2-digit number (01--12)                |
    | MMM       | the abbreviated English month name ("Jan"--"Dec")     |
    | DD        | the day as a 2-digit number (01--31)                  |
    | DDD       | the abbreviated English day of the week ("Mon"--"Sun")|
    | AP        | either "AM" or "PM"                                   |
    | ap        | either "am" or "pm"                                   |
    | hh        | the hour as a 2-digit number (00--23 or 01--12)       |
    | mm        | the minute as a 2-digit number (00--59)               |
    | ss        | the second as a 2-digit number (00--59)               |
    | TZD       |                                                       |
    | MSTREN    | English month name                                    |
    | DSTREN    | English day of the week                               |
    |           |                                                       |
    | MSTRUA    | Ukranian month name                                   |
    | DSTRUA    | Ukranian day of the week                              |
    |           |                                                       |
    | MSTRRU    | Russian month name                                    |
    | DSTRRU    | Russian day of the week                               |
    |           |                                                       |
    | MEN       | English short month name                              |
    | DEN       | English short day of the week                         |
    |           |                                                       |
    | MUA       | Ukranian short month name                             |
    | DUA       | Ukranian short day of the week                        |
    |           |                                                       |
    | MRU       | Russian short month name                              |
    | DRU       | Russian short day of the week                         |
*/
    static String unixGMTtoString(unix_t unix, const String& Format, int16_t tzM = 0) {
        unix += (tzM * 60);
        
        uint8_t day;
        uint8_t month;
        year_t year;
        uint8_t dayW = dayWeek(unix);
        
        uint8_t h, m, sec;
        
        decodeUNIX(unix, day, month, year, h, m, sec);
        
        String s = Format;
        String s2;
        
        if (year < 1000) s2 += F("0");
        if (year < 100) s2 += F("0");
        if (year < 10) s2 += F("0");
        s2 += year;
        s.replace(F("YYYY"), s2);
        s2 = "";
        
        if (year%100 < 10) s2 += F("0");
        s2 += year;
        s.replace(F("YY"), s2);
        s2 = "";
        
        if (month < 10) s2 += F("0");
        s2 += month;
        s.replace(F("MM"), s2);
        s2 = "";
        
        if (day < 10) s2 += F("0");
        s2 += day;
        s.replace(F("DD"), s2);
        s2 = "";
        
        s.replace(F("MEN"), monthShortString(month));
        s.replace(F("DEN"), dayWeekShortString(dayW));
        
        s.replace(F("MUA"), monthShortStringUA(month));
        s.replace(F("DUA"), dayWeekShortStringUA(dayW));
        
        s.replace(F("MRU"), monthShortStringRU(month));
        s.replace(F("DRU"), dayWeekShortStringRU(dayW));
        
        s.replace(F("MSTREN"), monthString(month));
        s.replace(F("DSTREN"), dayWeekString(dayW));
        
        s.replace(F("MSTRUA"), monthStringUA(month));
        s.replace(F("DSTRUA"), dayWeekStringUA(dayW));
        
        s.replace(F("MSTRRU"), monthStringRU(month));
        s.replace(F("DSTRRU"), dayWeekStringRU(dayW));
        
        if (isPM(h)) {
            s.replace(F("AP"), String(F("PM")));
            s.replace(F("ap"), String(F("pm")));
        } else {
            s.replace(F("AP"), String(F("AM")));
            s.replace(F("ap"), String(F("am")));
        }
        
        if (Format.indexOf(F("AP")) != -1 || Format.indexOf(F("ap")) != -1) {
            uint8_t hf = hourFormat12(h);
            if (hf < 10) s2 += F("0");
            s2 += hf;
        } else {
            if (h < 10) s2 += F("0");
            s2 += h;
        }
        s.replace(F("hh"), s2);
        s2 = "";
        
        if (m < 10) s2 += F("0");
        s2 += m;
        s.replace(F("mm"), s2);
        s2 = "";
        
        if (sec < 10) s2 += F("0");
        s2 += sec;
        s.replace(F("ss"), s2);
        s2 = "";
        
        int8_t tz = tzM / 60;
        tzM %= 60;
        if (tzM < 0) s2 += F("-");
        else s2 += F("+");
        if (abs(tz) < 10) s2 += F("0");
        s2 += abs(tz);
        
        s2 += F(":");
        
        if (abs(tzM) < 10) s2 += F("0");
        s2 += abs(tzM);
        s.replace(F("TZD"), s2);
        s2 = "";
        
        return s;
    }
    
    // отримати рядок часу формата ГГ:ХХ:СС
    static String timeString(uint8_t h, uint8_t m, uint8_t s) {
        String str;
        str.reserve(8);
        if (h < 10) str += F("0");
        str += h;
        str += F(":");
        if (m < 10) str += F("0");
        str += m;
        str += F(":");
        if (s < 10) str += F("0");
        str += s;
        return str;
    }

    // отримати рядок дати формата ДД.ММ.РРРР
    static String dateString(uint8_t d, uint8_t m, year_t y) {
        String str;
        str.reserve(10);
        if (d < 10) str += F("0");
        str += d;
        str += F(".");
        if (m < 10) str += F("0");
        str += m;
        str += F(".");
        str += y;
        return str;
    }
    
    //========================================================================
    static String monthString(uint8_t m) {
        String str = FPSTR(UNIX_MONTH_NAMES_EN[m-1]);
        return str;
    }
    
    //
    static String dayWeekString(uint8_t wd) {
        String str = FPSTR(UNIX_DAY_NAMES_EN[wd-1]);
        return str;
    }
    
    //
    static String monthShortString(uint8_t m) {
        String str = FPSTR(UNIX_MONTH_SHORT_NAMES_EN[m-1]);
        return str;
    }
    
    //
    static String dayWeekShortString(uint8_t wd) {
        String str = FPSTR(UNIX_DAY_SHORT_NAMES_EN[wd-1]);
        return str;
    }
    
    //=========================================================================
    static String monthStringUA(uint8_t m) {
        String str = FPSTR(UNIX_MONTH_NAMES_UA[m-1]);
        return str;
    }
    
    //
    static String dayWeekStringUA(uint8_t wd) {
        String str = FPSTR(UNIX_DAY_NAMES_UA[wd-1]);
        return str;
    }
    
    //
    static String monthShortStringUA(uint8_t m) {
        String str = FPSTR(UNIX_MONTH_SHORT_NAMES_UA[m-1]);
        return str;
    }
    
    //
    static String dayWeekShortStringUA(uint8_t wd) {
        String str = FPSTR(UNIX_DAY_SHORT_NAMES_UA[wd-1]);
        return str;
    }
    
    //=========================================================================
    static String monthStringRU(uint8_t m) {
        String str = FPSTR(UNIX_MONTH_NAMES_RU[m-1]);
        return str;
    }
    
    //
    static String dayWeekStringRU(uint8_t wd) {
        String str = FPSTR(UNIX_DAY_NAMES_RU[wd-1]);
        return str;
    }
    
    //
    static String monthShortStringRU(uint8_t m) {
        String str = FPSTR(UNIX_MONTH_SHORT_NAMES_RU[m-1]);
        return str;
    }
    
    //
    static String dayWeekShortStringRU(uint8_t wd) {
        String str = FPSTR(UNIX_DAY_SHORT_NAMES_RU[wd-1]);
        return str;
    }//=========================================================================
    
};

#ifndef NO_GLOBAL_TimeFunc
static TimeFunctions TimeFunc;
#endif

#endif