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

class UNIXtime {
public:
	UNIXtime (uint32_t unix = 0, int8_t tz = 0, int8_t tzM = 0, uint16_t ms = 0) {
		setUnixGMT(unix, ms);
		setTimeZone(tz, tzM);
	}
	
	UNIXtime (uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM = 0, uint16_t ms = 0) {
		setUnixFromDate(y, m, d, h, mn, s, tzM, ms);
	}
	
	// встановити unix час відносно грінвіча
    void setUnixGMT(uint32_t unix, uint16_t ms = 0) {
        _unix = unix;
		if (ms > 1000) {
			_unix += ms / 1000;
			ms = ms % 1000;
		}
		_ms = ms;
		_last_upd = millis() - _ms;
		
		setTimeStat(1);
		decodeTime();
    }
	
	// встановити unix час відносно грінвіча
    void setUnixFromDate(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM = 0, uint16_t ms = 0) {
		if (ms > 1000) {
			s += ms / 1000;
			ms = ms % 1000;
		}
		int my = (m >= 3) ? 1 : 0;
		y += my - 1970;
		uint16_t dm = 0;
		for (int i = 0; i < m - 1; i++) dm += (i<7)?((i==1)?28:((i&1)?30:31)):((i&1)?31:30);
        _unix = (((d-1+dm+((y+1)>>2)-((y+69)/100)+((y+369)/100/4)+365*(y-my))*24ul+h-(tzM/60))*60ul+mn-(tzM%60))*60ul+s;
		_ms = ms;
		_last_upd = millis() - _ms;
		_tz = tzM;
		
		setTimeStat(1);
		decodeTime();
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
	
	bool DST (uint8_t month_start, uint8_t week_start, uint8_t dayWeek_start, uint8_t h_start,
	uint8_t month_end, uint8_t week_end, uint8_t dayWeek_end, uint8_t h_end, bool dst_or_wt = 1) {
		if (_dst_wt_year != year()) {
			_dst_wt_year = year();
			UNIXtime c;
			
			uint8_t i;
			uint8_t I;
			//start
			if (!week_start) i = TimeFunc.lastDayOfMonth(month_start, _dst_wt_year) - 6;
			else i = (week_start - 1) * 7 + 1;
			
			if (!week_start) I = TimeFunc.lastDayOfMonth(month_start, _dst_wt_year) + 1;
			else I = week_start* 7 + 1;

			for (i; i < I; i++) {
				c.setUnixFromDate(_dst_wt_year, month_start, i, h_start, 0, 0, getTimeZoneM());
				if (c.dayWeek() == dayWeek_start) {
					_dst_wt_day_Start = c.day();
					break;
				}
			}
			//end			
			if (!week_end) i = TimeFunc.lastDayOfMonth(month_end, _dst_wt_year) - 6;
			else i = (week_end - 1) * 7 + 1;
			
			if (!week_end) I = TimeFunc.lastDayOfMonth(month_end, _dst_wt_year) + 1;
			else I = week_end* 7 + 1;

			for (i; i < I; i++) {
				c.setUnixFromDate(_dst_wt_year, month_end, i, h_end, 0, 0, getTimeZoneM() + (60 * (dst_or_wt ? 1 : -1)));
				if (c.dayWeek() == dayWeek_end) {
					_dst_wt_day_End = c.day();
					break;
				}
			}
		}

		if (getDST() == 0 && onDate(_dst_wt_day_Start, month_start, _dst_wt_year) && onTime(h_start, 0, 0)){
			if (dst_or_wt) setSummerTime();
			else setWinterTime();
			return 1;
		}
		else if (getDST() == (dst_or_wt ? 1 : -1) && onDate(_dst_wt_day_End, month_end, _dst_wt_year) && onTime(h_end, 0, 0)) {
			setStandardTime();
			return 1;
		}
		
		else if (month() >= month_start && month() <= month_end &&
			unix() >= UNIXtime(_dst_wt_year, month_start, _dst_wt_day_Start, h_start, 0, 0, getTimeZoneM()).unix() &&
			unix() < UNIXtime(_dst_wt_year, month_end, _dst_wt_day_End, h_end, 0, 0, getTimeZoneM() + (60 * (dst_or_wt ? 1 : -1))).unix()) {
			if (getDST() == 0) {
				if (dst_or_wt) setSummerTime();
				else setWinterTime();
				return 1;
			}
		} else if (getDST() == (dst_or_wt ? 1 : -1)) {
			setStandardTime();
			return 1;
		}
		
		return 0;
	}
	
	// отримати статус системи
    /*
        UNIX_OK 				0 - все ок
        UNIX_NOT_SYNCHRONIZED 	1 - не синхронізовано
        UNIX_NOT_STARTED 		2 - зупинено
    */	
	bool status() {
        return _time_stat;
    }
	
	//
	void startTime() {
		if (!_time) {
			_time = true;
			_last_upd = millis() - _ms;
			_time_stat = 1;
		}
	}
	
	//
	void stopTime() {
		if (_time) {
			_time = false;
			updateUNIX();
			_time_stat = 2;
		}
	}
	
	//
	UNIXtime operator=(const UNIXtime& _t) {
		_unix = _t._unix;
		_last_upd = _t._last_upd;
		
		if (_t._time) updateUNIX();
		else {
			_ms = _t._ms;
			_last_upd = millis() - _ms;
		}
		
		_tz = _t._tz;
		_dst_wt = _t._dst_wt;
		
		setTimeStat(1);
		decodeTime();
		
		return *this;
    }
	
	//
	UNIXtime operator=(const uint32_t _t) {
		setUnixGMT(_t);
		return *this;
    }


    // мілісекунд з останнього оновлення
    uint32_t msFromUpdate() {
		if (_time) {
			// захист від переповнення різниці через 50 діб
			uint32_t diff = millis() - _last_upd;
			if (diff > 129600000UL) {	//1,5 доби
				_unix += diff / 1000UL;
				_last_upd = millis() - diff % 1000UL;
			}
			return diff;
		} else {
			return _ms;
		}
    }

    // unix час відносно грінвіча
    uint32_t unixGMT() {
        return _unix + (msFromUpdate() / 1000UL);
    }
	
	// unix час відносно даного time zone
    uint32_t unix() {
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
    uint16_t year() {
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
	bool onDate (uint8_t d, uint8_t m, uint8_t y) {
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
	bool dateAfter (uint8_t d, uint8_t m, uint8_t y) {
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
	uint32_t periodInSec(uint32_t last_unix) {
		return TimeFunc.periodInSec(unix(), last_unix);
	}
	
	//
	uint16_t periodInFullDays(uint32_t last_unix) {
		return TimeFunc.periodInFullDays(unix(), last_unix);
	}
	
	//
	uint16_t periodInDays(uint32_t last_unix) {
		return TimeFunc.periodInDays(unix(), last_unix);
	}
	
	//
	uint16_t periodInMonths(uint32_t last_unix) {
		return TimeFunc.periodInMonths(unix(), last_unix);
	}
	
	//========================================================================
	bool timeOutMonth(uint32_t last_unix, uint16_t time_out) {
		return TimeFunc.timeOutMonth(unix(), last_unix, time_out);
	}
	
	//
	bool timeOutD(uint32_t last_unix, uint16_t time_out) {
		return TimeFunc.timeOutD(unix(), last_unix, time_out);
	}
	
	//
	bool timeOutFullDays(uint32_t last_unix, uint16_t time_out) {
		return TimeFunc.timeOutFullDays(unix(), last_unix, time_out);
	}
	
	//
	bool timeOutH(uint32_t last_unix, uint16_t time_out) {
		return TimeFunc.timeOutH(unix(), last_unix, time_out);
	}
	
	//
	bool timeOutM(uint32_t last_unix, uint16_t time_out) {
		return TimeFunc.timeOutM(unix(), last_unix, time_out);
	}
	
	//
	bool timeOutS(uint32_t last_unix, uint16_t time_out) {
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
		if (_prev_calc == unix() / 86400ul) return;
		_prev_calc = unix() / 86400ul;
        // http://howardhinnant.github.io/date_algorithms.html#civil_from_days
        uint32_t u = _prev_calc;
        u += 719468;
        uint8_t era = u / 146097ul;
        uint16_t doe = u - era * 146097ul;
        uint16_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
        _year = yoe + era * 400;
        uint16_t doy = doe - (yoe * 365 + yoe / 4 - yoe / 100);
        uint16_t mp = (doy * 5 + 2) / 153;
        _day = doy - (mp * 153 + 2) / 5 + 1;
        _month = mp + (mp < 10 ? 3 : -9);
        _year += (_month <= 2);
    }
	uint16_t _prev_calc = 0;
	
	uint8_t _day : 5;
	uint8_t	_month : 4;
    uint16_t _year : 12;
	
	void updateUNIX() {
		uint32_t diff = millis() - _last_upd;
		_unix += diff / 1000ul;
		_ms = diff % 1000ul;
		_last_upd = millis() - _ms;
	}

protected:
	void setTimeStat(uint8_t s) {
		if (_time) _time_stat = s;
		else _time_stat = 2;
	}

	int16_t _tz = 0;
	int8_t _dst_wt : 2 = 0;
	
	uint16_t _dst_wt_year : 12;
	uint8_t _dst_wt_day_Start: 5;
	uint8_t _dst_wt_day_End : 5;
	
	bool _time : 1 = 0;
	uint8_t _time_stat : 2 = 2;

	uint32_t _last_upd = 0;
	uint32_t _unix = 0;
	uint16_t _ms : 10 = 0;
	
};
#endif