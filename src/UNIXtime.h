/*

Денис Чухліб ,https://github.com/DenysChuhlib
MIT License

*/

#ifndef UNIXtime_h
#define UNIXtime_h

#define UNIX_OK 0
#define UNIX_NOT_SYNCHRONIZED 1
#define UNIX_NOT_STARTED 2

#include "TimeFunc.h"

class DSTime {
public:
	DSTime() {}
	DSTime(uint8_t month_start_, uint8_t week_start_, uint8_t dayWeek_start_, uint8_t h_start_,
		uint8_t month_end_, uint8_t week_end_, uint8_t dayWeek_end_, uint8_t h_end_, bool dst_or_wt_ = 1) :
		month_start(month_start_), week_start(week_start_), dayWeek_start(dayWeek_start_), h_start(h_start_),
		month_end(month_end_), week_end(week_end_), dayWeek_end(dayWeek_end_), h_end(h_end_), dst_or_wt(dst_or_wt_) {}

	void setDST(uint8_t month_start_, uint8_t week_start_, uint8_t dayWeek_start_, uint8_t h_start_,
		uint8_t month_end_, uint8_t week_end_, uint8_t dayWeek_end_, uint8_t h_end_, bool dst_or_wt_ = 1) {
		month_start = month_start_;
		week_start = week_start_;
		dayWeek_start = dayWeek_start_;
		h_start = h_start_;
		
		month_end = month_end_; 
		week_end = week_end_;
		dayWeek_end = dayWeek_end_;
		h_end = h_end_;
		
		dst_or_wt = dst_or_wt_;
	}
	
	uint8_t month_start		: 4;
	uint8_t week_start		: 3;
	uint8_t dayWeek_start	: 3;
	uint8_t h_start			: 5;
	                      
	uint8_t month_end		: 4;
	uint8_t week_end		: 3;
	uint8_t dayWeek_end		: 3;
	uint8_t h_end			: 5;

	uint8_t dst_or_wt		: 1 = 1;
};

class UNIXtime {
public:
	UNIXtime (unix_t unix = 0, int8_t tz = 0, int8_t tzM = 0, uint16_t ms = 0) {
		setUnixGMT(unix, ms);
		setTimeZone(tz, tzM);
	}
	
	UNIXtime (year_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM = 0, uint16_t ms = 0) {
		setUnixFromDate(y, m, d, h, mn, s, tzM, ms);
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
    void setUnixFromDate(year_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM = 0, uint16_t ms = 0) {
		if (ms > 1000) {
			s += ms / 1000;
			ms = ms % 1000;
		}
		
        _unix = TimeFunc.unixGMTFromDate(y, m, d, h, mn, s, tzM);
		
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
		_dst_wt = 1;
	}
	
	// установити літній час
    void setSummerTime () {
		_dst_wt = 1;
	}
	
	// установити зимовий час
    void setWinterTime () {
		_dst_wt = -1;
	}
	
	// установити стандартний час
    void setStandardTime () {
		_dst_wt = 0;
	}
	
	//
	int8_t getDST () {
		return _dst_wt;
	}
	
	void setDSTauto(DSTime * dst) {
		_dst = dst;
	}
	
	void delDSTauto() {
		_dst = nullptr;
	}
	
	bool DSTauto() {
		return (_dst) ? DST(*_dst) : 0;
	}
	
	bool DST (DSTime & dst) {
		return DST(dst.month_start, dst.week_start, dst.dayWeek_start, dst.h_start,
			dst.month_end, dst.week_end, dst.dayWeek_end, dst.h_end, dst.dst_or_wt);
	}
	
	bool DST (uint8_t month_start, uint8_t week_start, uint8_t dayWeek_start, uint8_t h_start,
	uint8_t month_end, uint8_t week_end, uint8_t dayWeek_end, uint8_t h_end, bool dst_or_wt = 1) {
		_dst_start = 1;

		if (unixGMT() >= DSTstartInGMT(month_start, week_start, dayWeek_start, h_start) &&
			unixGMT() < DSTendInGMT(month_end, week_end, dayWeek_end, h_end, dst_or_wt)) {
			if (getDST() == 0) {
				if (dst_or_wt) setSummerTime();
				else setWinterTime();
				
				_dst_start = 0;
				return 1;
			}
		} else if (getDST() == (dst_or_wt ? 1 : -1)) {
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
		if (!week_start) i = TimeFunc.lastDayOfMonth(month_start, year()) - 6;
		else i = (week_start - 1) * 7 + 1;
		
		unix_t unix = TimeFunc.unixGMTFromDate(year(), month_start, i, h_start, 0, 0, getTimeZoneM());
		a = (int8_t)TimeFunc.dayWeek(unix) - dayWeek_start;
		if (a < 0) a = -a;
		i += a;
		if (a) unix = TimeFunc.unixGMTFromDate(year(), month_start, i, h_start, 0, 0, getTimeZoneM());

		return unix;
	}
	
	unix_t DSTendInGMT (uint8_t month_end, uint8_t week_end, uint8_t dayWeek_end, uint8_t h_end, bool dst_or_wt = 1) {
		uint8_t i;
		int8_t a;
		int8_t T = (getDST() != 0) ? 0 : 60 * (dst_or_wt ? 1 : -1);
		//end
		if (!week_end) i = TimeFunc.lastDayOfMonth(month_end, year()) - 6;
		else i = (week_end - 1) * 7 + 1;
		
		unix_t unix = TimeFunc.unixGMTFromDate(year(), month_end, i, h_end, 0, 0, getTimeZoneM() + T);
		a = (int8_t)TimeFunc.dayWeek(unix) - dayWeek_end;
		if (a < 0) a = -a;
		i += a;
		if (a) unix = TimeFunc.unixGMTFromDate(year(), month_end, i, h_end, 0, 0, getTimeZoneM() + T);
		
		return unix;
	}
	
	// отримати статус системи
    /*
        UNIX_OK 				0 - все ок
        UNIX_NOT_SYNCHRONIZED 	1 - не синхронізовано
        UNIX_NOT_STARTED 		2 - зупинено
    */	
	uint8_t status() {
        return _time_stat = _time ? _time_stat : 2;
    }
	
	bool isRunning() {
        return _time;
    }
	
	//
	void startTime() {
		if (!_time) {
			_time = true;
			_ms = millis() - _ms;
			_time_stat = 1;
		}
	}
	
	//
	void stopTime() {
		if (_time) {
			_time = false;
			uint32_t diff = millis() - _ms;
			_unix += diff / 1000ul;
			_ms = diff % 1000ul;
			_time_stat = 2;
		}
	}


    // мілісекунд з останнього оновлення
    uint32_t msFromUpdate() {
		if (_time) {
			// захист від переповнення різниці через 50 діб
			uint32_t diff = millis() - _ms;
			if (diff > 129600000UL) {	//1,5 доби
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
        uint8_t _dayw = ((unix() / 86400UL) + 4) % 7;
        if (!_dayw) _dayw = 7;
        return _dayw;
    }
	
	//
	bool isAM() {
		return TimeFunc.isAM(hour());
	}
	
	//
	bool isPM() {
		return TimeFunc.isPM(hour());
	}
	
	//
	uint8_t hourFormat12() {
		return TimeFunc.hourFormat12(hour());
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
		return TimeFunc.everyH(hour(), time_last, time_out);
	}
	
	//
	bool everyM(uint8_t time_last, uint8_t time_out) {
		return TimeFunc.everyM(minute(), time_last, time_out);
	}
	
	//
	bool everyS(uint8_t time_last, uint8_t time_out) {
		return TimeFunc.everyS(second(), time_last, time_out);
	}
	
	//
	bool everyMs(uint16_t time_last, uint16_t time_out) {
		return TimeFunc.everyMs(ms(), time_last, time_out);
	}
	
	//========================================================================
	unix_t periodInSec(unix_t last_unix) {
		return TimeFunc.periodInSec(unix(), last_unix);
	}
	
	//
	unix_t periodInFullDays(unix_t last_unix) {
		return TimeFunc.periodInFullDays(unix(), last_unix);
	}
	
	//
	unix_t periodInDays(unix_t last_unix) {
		return TimeFunc.periodInDays(unix(), last_unix);
	}
	
	//
	unix_t periodInMonths(unix_t last_unix) {
		return TimeFunc.periodInMonths(unix(), last_unix);
	}
	
	//========================================================================
	bool timeOutMonth(unix_t last_unix, unix_t time_out) {
		return TimeFunc.timeOutMonth(unix(), last_unix, time_out);
	}
	
	//
	bool timeOutD(unix_t last_unix, unix_t time_out) {
		return TimeFunc.timeOutD(unix(), last_unix, time_out);
	}
	
	//
	bool timeOutFullDays(unix_t last_unix, unix_t time_out) {
		return TimeFunc.timeOutFullDays(unix(), last_unix, time_out);
	}
	
	//
	bool timeOutH(unix_t last_unix, unix_t time_out) {
		return TimeFunc.timeOutH(unix(), last_unix, time_out);
	}
	
	//
	bool timeOutM(unix_t last_unix, unix_t time_out) {
		return TimeFunc.timeOutM(unix(), last_unix, time_out);
	}
	
	//
	bool timeOutS(unix_t last_unix, unix_t time_out) {
		return TimeFunc.timeOutS(unix(), last_unix, time_out);
	}
	
	bool isLeap() {
		return TimeFunc.isLeap(year());
	}
	
	uint8_t lastDayOfMonth() {
		return TimeFunc.lastDayOfMonth(month(), year());
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
	
    // отримати рядок часу формата ГГ:ХХ:СС
    String timeString() {
		return TimeFunc.timeString(hour(), minute(), second());
    }

    // отримати рядок дати формата ДД.ММ.РРРР
    String dateString() {
		decodeTime();
		return TimeFunc.dateString(_day, _month, _year);
    }
	
	//========================================================================
    String monthString() {
		return TimeFunc.monthString(month());
    }
	
	//
    String dayWeekString() {
		return TimeFunc.dayWeekString(dayWeek());
    }
	
	//
    String monthShortString() {
		return TimeFunc.monthShortString(month());
    }
	
	//
    String dayWeekShortString() {
		return TimeFunc.dayWeekShortString(dayWeek());
    }
	
	//========================================================================
    String monthStringUA() {
		return TimeFunc.monthStringUA(month());
    }
	
	//
    String dayWeekStringUA() {
		return TimeFunc.dayWeekStringUA(dayWeek());
    }
	
	//
    String monthShortStringUA() {
		return TimeFunc.monthShortStringUA(month());
    }
	
	//
    String dayWeekShortStringUA() {
		return TimeFunc.dayWeekShortStringUA(dayWeek());
    }
	
	//========================================================================
	String monthStringRU() {
		return TimeFunc.monthStringRU(month());
    }
	
	//
    String dayWeekStringRU() {
		return TimeFunc.dayWeekStringRU(dayWeek());
    }
	
	//
    String monthShortStringRU() {
		return TimeFunc.monthShortStringRU(month());
    }
	
	//
    String dayWeekShortStringRU() {
		return TimeFunc.dayWeekShortStringRU(dayWeek());
    }//========================================================================
	
	
private:
	// перерахунок unix в дату
    void decodeTime() {
		if (_prev_calc == unix() / 86400) return;
		_prev_calc = unix() / 86400;
		uint8_t day;
		uint8_t	month;
		year_t year;
		
        TimeFunc.decodeUNIX(unix(), day, month, year);
		
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
	uint8_t	_month : 4;

	bool _dst_start : 1;
	
	bool _time : 1 = 0;
	uint8_t _time_stat : 2 = 2;	
	
	int16_t _tz : 11 = 0;
	int8_t _dst_wt : 2 = 0;
	
	DSTime* _dst = nullptr;

protected:
	void setTimeStat(uint8_t s) {
		if (_time) _time_stat = s;
		else _time_stat = 2;
	}
	
	void timeChanged() {
		setTimeStat(1);
		decodeTime();
		DSTauto();
	}

	uint32_t _ms = 0;
	unix_t _unix = 0;
	
};
#endif