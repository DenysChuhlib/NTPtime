/*

Денис Чухліб ,https://github.com/DenysChuhlib
MIT License

*/

#ifndef UNIXtime_h
#define UNIXtime_h

enum UNIX_Status : uint8_t {
    UNIX_OK,                // 0 - все ок
    UNIX_NOT_SYNCHRONIZED,  // 1 - не синхронізовано
    UNIX_NOT_STARTED        // 2 - зупинено
};

#include "TimeFunc.h"

enum DSTimeMode : int8_t {
    DST_WT = -1,    //Winter time
    DST_OFF,        //None
    DST_ON          //DST
};

class DSTime {
public:
    DSTime() {
        mode = 0;
        month_start = 0;
        week_start = 0;
        dayWeek_start = 0;
        h_start = 0;
        month_end = 0;
        week_end = 0;
        dayWeek_end = 0;
        h_end = 0;
    }
    
    DSTime(uint8_t month_start_, uint8_t week_start_, uint8_t dayWeek_start_, uint8_t h_start_,
        uint8_t month_end_, uint8_t week_end_, uint8_t dayWeek_end_, uint8_t h_end_, int8_t mode_ = DST_ON) :
        month_start(month_start_), week_start(week_start_), dayWeek_start(dayWeek_start_), h_start(h_start_),
        month_end(month_end_), week_end(week_end_), dayWeek_end(dayWeek_end_), h_end(h_end_), mode(mode_) {}

    void setDST(uint8_t month_start_, uint8_t week_start_, uint8_t dayWeek_start_, uint8_t h_start_,
        uint8_t month_end_, uint8_t week_end_, uint8_t dayWeek_end_, uint8_t h_end_, int8_t mode_ = DST_ON) {
        month_start = month_start_;
        week_start = week_start_;
        dayWeek_start = dayWeek_start_;
        h_start = h_start_;
        
        month_end = month_end_; 
        week_end = week_end_;
        dayWeek_end = dayWeek_end_;
        h_end = h_end_;
        
        mode = mode_;
    }
    
    uint8_t month_start     : 4;
    uint8_t week_start      : 3;
    uint8_t dayWeek_start   : 3;
    uint8_t h_start         : 5;
                          
    uint8_t month_end       : 4;
    uint8_t week_end        : 3;
    uint8_t dayWeek_end     : 3;
    uint8_t h_end           : 5;

    int8_t mode             : 2;
};

class UNIXtime {
public:
    UNIXtime (unix_t unix = 0, int8_t tz = 0, int8_t tzM = 0, uint16_t ms = 0) {
        setUnixGMT(unix, ms);
        setTimeZone(tz, tzM);
        
        _time_stat = UNIX_NOT_STARTED;
    }
    
    UNIXtime (year_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM = 0, uint16_t ms = 0) {
        setUnixGMTFromDate(y, m, d, h, mn, s, tzM, ms);
        
        _time_stat = UNIX_NOT_STARTED;
    }
    
    // встановити unix час відносно грінвіча
    void setUnixGMT(unix_t unix, uint16_t ms = 0) {
        _unix = unix;
        if (ms > 1000) {
            _unix += ms / 1000;
            ms = ms % 1000;
        }
        
        if (_time) _ms = millis() - ms;
        else _ms = ms;
        
        timeChanged();
    }
    
    // встановити unix час відносно грінвіча
    void setUnixGMTFromDate(year_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM = 0, uint16_t ms = 0) {
        if (ms > 1000) {
            s += ms / 1000;
            ms = ms % 1000;
        }
        
        _unix = TimeFunctions::unixGMTFromDate(y, m, d, h, mn, s, tzM);
        
        if (_time) _ms = millis() - ms;
        else _ms = ms;
        
        _tz = tzM;
        
        timeChanged();
    }
    
    void addUnix(unix_t unix, uint16_t ms = 0) {
        _unix += unix;
        if (ms > 1000) {
            _unix += ms / 1000;
            ms = ms % 1000;
        }
        
        uint16_t mss = _ms;
        mss += ms;
        if (mss >= 1000) {
            _unix++;
            _ms = mss - 1000;
        }
        
        timeChanged();
    }
    
    void subtractUnix(unix_t unix, uint16_t ms = 0) {
        _unix -= unix;
        if (ms > 1000) {
            _unix -= ms / 1000;
            ms = ms % 1000;
        }
        
        int16_t mss = _ms;
        mss -= ms;
        if (mss < 0) {
            _unix--;
            _ms = mss + 1000;
        }
        
        timeChanged();
    }
    
    //
    UNIXtime& operator = (const UNIXtime& _t) {
        _unix = _t._unix;
        _ms = _t._ms;
        
        if (_t._time) {
            uint32_t diff = millis() - _ms;
            _unix += diff / 1000ul;
            if (_time) _ms = millis() - diff % 1000ul;
            else _ms = diff % 1000ul;
        } else {
            if (_time) _ms = millis() - _ms;
        }
        
        _tz = _t._tz;
        _dst = _t._dst;
        
        timeChanged();
        
        return *this;
    }
    
    UNIXtime& operator = (unix_t _t) {
        setUnixGMT(_t);
        return *this;
    }
    
    ~UNIXtime() {
    }

    // установити часовий пояс
    void setTimeZone(int8_t tz, int16_t tzM = 0) {
        _tz = tz * 60UL;
        _tz += tzM;
    }
    
    //
    int16_t getTimeZoneM () {
        return _tz + (_dst_wt * 60UL);
    }
    
    // установити літній час
    void setDST () {
        _dst_wt = DST_ON;
    }
    
    // установити літній час
    void setSummerTime () {
        _dst_wt = DST_ON;
    }
    
    // установити зимовий час
    void setWinterTime () {
        _dst_wt = DST_WT;
    }
    
    // установити стандартний час
    void setStandardTime () {
        _dst_wt = DST_OFF;
    }
    
    //
    int8_t getDST () {
        return _dst_wt;
    }
    
    void setDSTauto(DSTime * dst) {
        _dst = dst;
    }
    
    DSTime *getDSTauto() {
        return _dst;
    }
    
    void delDSTauto() {
        _dst = nullptr;
    }
    
    bool DSTauto() {
        return (_dst) ? DST(*_dst) : 0;
    }
    
    bool DST (DSTime & dst) {
        if (dst.mode != DST_OFF) {
            return DST(dst.month_start, dst.week_start, dst.dayWeek_start, dst.h_start,
            dst.month_end, dst.week_end, dst.dayWeek_end, dst.h_end, (dst.mode == DST_ON) ? 1 : 0);
        } else if (getDST() != DST_OFF) {
            setStandardTime();
            return true;
        }
        return false;
    }
    
    bool DST (uint8_t month_start, uint8_t week_start, uint8_t dayWeek_start, uint8_t h_start,
    uint8_t month_end, uint8_t week_end, uint8_t dayWeek_end, uint8_t h_end, bool dst_or_wt = 1) {
        _dst_start = 1;

        if (unixGMT() >= DSTstartInGMT(month_start, week_start, dayWeek_start, h_start) &&
            unixGMT() < DSTendInGMT(month_end, week_end, dayWeek_end, h_end, dst_or_wt)) {
            if (getDST() == DST_OFF) {
                if (dst_or_wt) setSummerTime();
                else setWinterTime();
                
                _dst_start = 0;
                return 1;
            }
        } else if (getDST() == (dst_or_wt ? DST_ON : DST_WT)) {
            setStandardTime();
            
            _dst_start = 0;
            return 1;
        }
        
        _dst_start = 0;
        return 0;
    }
    
    unix_t DSTstartInGMT (uint8_t month_start, uint8_t week_start, uint8_t dayWeek_start, uint8_t h_start) {
        uint8_t i;
        int8_t a;
        //start
        if (!week_start) i = TimeFunctions::lastDayOfMonth(month_start, year()) - 6;
        else i = (week_start - 1) * 7 + 1;
        
        unix_t unix = TimeFunctions::unixGMTFromDate(year(), month_start, i, h_start, 0, 0, getTimeZoneM());
        a = (int8_t)TimeFunctions::dayWeek(unix) - dayWeek_start;
        if (a < 0) a = -a;
        if (a) unix += a * 86400;

        return unix;
    }
    
    unix_t DSTendInGMT (uint8_t month_end, uint8_t week_end, uint8_t dayWeek_end, uint8_t h_end, bool dst_or_wt = 1) {
        uint8_t i;
        int8_t a;
        int8_t T = (getDST() != DST_OFF) ? 0 : 60 * (dst_or_wt ? DST_ON : DST_WT);
        //end
        if (!week_end) i = TimeFunctions::lastDayOfMonth(month_end, year()) - 6;
        else i = (week_end - 1) * 7 + 1;
        
        unix_t unix = TimeFunctions::unixGMTFromDate(year(), month_end, i, h_end, 0, 0, getTimeZoneM() + T);
        a = (int8_t)TimeFunctions::dayWeek(unix) - dayWeek_end;
        if (a < 0) a = -a;
        if (a) unix += a * 86400;
        
        return unix;
    }
    
    // отримати статус системи
    /*
        UNIX_OK                 0 - все ок
        UNIX_NOT_SYNCHRONIZED   1 - не синхронізовано
        UNIX_NOT_STARTED        2 - зупинено
    */  
    uint8_t status() {
        return _time_stat = _time ? _time_stat : UNIX_NOT_STARTED;
    }
    
    bool isRunning() {
        return _time;
    }
    
    //
    void startTime() {
        if (!_time) {
            _time = true;
            _ms = millis() - _ms;
            _time_stat = UNIX_NOT_SYNCHRONIZED;
        }
    }
    
    //
    void stopTime() {
        if (_time) {
            _time = false;
            uint32_t diff = millis() - _ms;
            _unix += diff / 1000ul;
            _ms = diff % 1000ul;
            _time_stat = UNIX_NOT_STARTED;
        }
    }


    // мілісекунд з останнього оновлення
    uint32_t msFromUpdate() {
        if (_time) {
            // захист від переповнення різниці через 50 діб
            uint32_t diff = millis() - _ms;
            if (diff > 129600000UL) {   //1,5 доби
                _unix += diff / 1000UL;
                _ms = millis() - diff % 1000UL;
                setTimeStat(UNIX_NOT_SYNCHRONIZED);
            }
            if (!_dst_start) {
                DSTauto();
            }
            return diff;
        } else {
            return _ms;
        }
    }

    // unix час відносно грінвіча
    unix_t unixGMT() {
        return _unix + (msFromUpdate() / 1000UL);
    }
    
    // unix час відносно даного time zone
    unix_t unix() {
        return unixGMT() + _tz * 60UL + (_dst_wt * 3600UL);
    }
    
    // мілісекунди
    uint16_t ms() {
        return msFromUpdate() % 1000;
    }

    // отримати секунди
    uint8_t second() {
        return unix() % 60;
    }

    // отримати хвилини
    uint8_t minute() {
        return (unix() % 3600UL) / 60UL;
    }

    // отримати години
    uint8_t hour() {
        return (unix() % 86400UL) / 3600UL;
    }

    // отримати день місяця
    uint8_t day() {
        decodeTime();
        return _day;
    }

    // отримати місяць
    uint8_t month() {
        decodeTime();
        return _month;
    }

    // отримати рік
    year_t year() {
        decodeTime();
        return _year;
    }

    // отримати день тижня
    uint8_t dayWeek() {
        return TimeFunctions::dayWeek(unix());
    }
    
    //
    bool isAM() {
        return TimeFunctions::isAM(hour());
    }
    
    //
    bool isPM() {
        return TimeFunctions::isPM(hour());
    }
    
    //
    uint8_t hourFormat12() {
        return TimeFunctions::hourFormat12(hour());
    }
    
    //
    bool onTime (uint8_t h, uint8_t m, uint8_t s) {
        if (h == hour() && m == minute() && s == second()) return true;
        return false;
    }
    
    //
    bool onDate (uint8_t d, uint8_t m, year_t y) {
        decodeTime();
        if (d == _day && m == _month && y == _year) return true;
        return false;
    }
    
    //
    bool timeAfter (uint8_t h, uint8_t m, uint8_t s) {
        if (h < hour() || (h == hour() && m < minute() || (h == hour() && m == minute() && s < second()))) return true;
        return false;
    }
    
    //
    bool dateAfter (uint8_t d, uint8_t m, year_t y) {
        decodeTime();
        if (((d < _day && m == _month && y == _year) || m < _month && y == _year) || y < _year) return true;
        return false;
    }
    
    //========================================================================
    bool everyH(uint8_t time_last, uint8_t time_out) {
        return TimeFunctions::everyH(hour(), time_last, time_out);
    }
    
    //
    bool everyM(uint8_t time_last, uint8_t time_out) {
        return TimeFunctions::everyM(minute(), time_last, time_out);
    }
    
    //
    bool everyS(uint8_t time_last, uint8_t time_out) {
        return TimeFunctions::everyS(second(), time_last, time_out);
    }
    
    //
    bool everyMs(uint16_t time_last, uint16_t time_out) {
        return TimeFunctions::everyMs(ms(), time_last, time_out);
    }
    
    //========================================================================
    unix_t periodInSec(unix_t last_unix) {
        return TimeFunctions::periodInSec(unix(), last_unix);
    }
    
    //
    unix_t periodInFullDays(unix_t last_unix) {
        return TimeFunctions::periodInFullDays(unix(), last_unix);
    }
    
    //
    unix_t periodInDays(unix_t last_unix) {
        return TimeFunctions::periodInDays(unix(), last_unix);
    }
    
    //
    unix_t periodInMonths(unix_t last_unix) {
        return TimeFunctions::periodInMonths(unix(), last_unix);
    }
    
    //========================================================================
    bool timeOutMonth(unix_t last_unix, unix_t time_out) {
        return TimeFunctions::timeOutMonth(unix(), last_unix, time_out);
    }
    
    //
    bool timeOutD(unix_t last_unix, unix_t time_out) {
        return TimeFunctions::timeOutD(unix(), last_unix, time_out);
    }
    
    //
    bool timeOutFullDays(unix_t last_unix, unix_t time_out) {
        return TimeFunctions::timeOutFullDays(unix(), last_unix, time_out);
    }
    
    //
    bool timeOutH(unix_t last_unix, unix_t time_out) {
        return TimeFunctions::timeOutH(unix(), last_unix, time_out);
    }
    
    //
    bool timeOutM(unix_t last_unix, unix_t time_out) {
        return TimeFunctions::timeOutM(unix(), last_unix, time_out);
    }
    
    //
    bool timeOutS(unix_t last_unix, unix_t time_out) {
        return TimeFunctions::timeOutS(unix(), last_unix, time_out);
    }
    
    bool isLeap() {
        return TimeFunctions::isLeap(year());
    }
    
    uint8_t lastDayOfMonth() {
        return TimeFunctions::lastDayOfMonth(month(), year());
    }
    
    void delay(uint32_t t, void (*func)() = NULL) {
        uint32_t a = micros();
        while (t > 0) {
            if (*func) func();
            if (_time) msFromUpdate();
            while (t > 0 && (micros() - a) >= 1000) {
                t--;
                a += 1000;
            }
            yield();
        }
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
    String unixToString(const String& Format) {
        return TimeFunctions::unixGMTtoString(unixGMT(), Format, getTimeZoneM());
    }
    
    // отримати рядок часу формата ГГ:ХХ:СС
    String timeString() {
        return TimeFunctions::timeString(hour(), minute(), second());
    }

    // отримати рядок дати формата ДД.ММ.РРРР
    String dateString() {
        decodeTime();
        return TimeFunctions::dateString(_day, _month, _year);
    }
    
    //========================================================================
    String monthString() {
        return TimeFunctions::monthString(month());
    }
    
    //
    String dayWeekString() {
        return TimeFunctions::dayWeekString(dayWeek());
    }
    
    //
    String monthShortString() {
        return TimeFunctions::monthShortString(month());
    }
    
    //
    String dayWeekShortString() {
        return TimeFunctions::dayWeekShortString(dayWeek());
    }
    
    //========================================================================
    String monthStringUA() {
        return TimeFunctions::monthStringUA(month());
    }
    
    //
    String dayWeekStringUA() {
        return TimeFunctions::dayWeekStringUA(dayWeek());
    }
    
    //
    String monthShortStringUA() {
        return TimeFunctions::monthShortStringUA(month());
    }
    
    //
    String dayWeekShortStringUA() {
        return TimeFunctions::dayWeekShortStringUA(dayWeek());
    }
    
    //========================================================================
    String monthStringRU() {
        return TimeFunctions::monthStringRU(month());
    }
    
    //
    String dayWeekStringRU() {
        return TimeFunctions::dayWeekStringRU(dayWeek());
    }
    
    //
    String monthShortStringRU() {
        return TimeFunctions::monthShortStringRU(month());
    }
    
    //
    String dayWeekShortStringRU() {
        return TimeFunctions::dayWeekShortStringRU(dayWeek());
    }//========================================================================
    
    
private:
    // перерахунок unix в дату
    void decodeTime() {
        if (_prev_calc == unix() / 86400) return;
        _prev_calc = unix() / 86400;
        uint8_t day;
        uint8_t month;
        year_t year;
        
        TimeFunctions::decodeUNIX(unix(), day, month, year);
        
        _day = day;
        _month = month;
        _year = year;
    }
    
    year_t _prev_calc = 0;

#ifdef UNIX64
    year_t _year;
#else
    year_t _year : 12;
#endif
    
    uint8_t _day : 5;
    uint8_t _month : 4;

    bool _dst_start : 1;
    
    bool _time : 1;
    uint8_t _time_stat : 2; 
    
    int16_t _tz : 11;
    int8_t _dst_wt : 2;
    
    DSTime* _dst = nullptr;

protected:
    void setTimeStat(uint8_t s) {
        if (_time) _time_stat = s;
        else _time_stat = UNIX_NOT_STARTED;
    }
    
    void timeChanged() {
        setTimeStat(UNIX_NOT_SYNCHRONIZED);
        decodeTime();
        DSTauto();
    }

    uint32_t _ms = 0;
    unix_t _unix = 0;
    
};
#endif