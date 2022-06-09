/*

Денис Чухліб ,https://github.com/DenysChuhlib
MIT License

*/

#ifndef UNIXtime_h
#define UNIXtime_h

#define UNIX_OK 0
#define UNIX_NOT_SYNCHRONIZED 1
#define UNIX_NOT_STARTED 2

const PROGMEM uint8_t UNIX_LAST_DAY_OF_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#include "languages/en.h"
#include "languages/ua.h"

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
    void setTimeZone(int8_t tz, int8_t tzM = 0) {
        _tz = tz * 60L;
		_tz += tzM;
    }
	
	//
	int16_t getTimeZoneM () {
		return _tz + (_dst_wt * 60);
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
			if (!week_start) i = lastDayOfMonth(month_start) - 6;
			else i = (week_start - 1) * 7 + 1;
			
			if (!week_start) I = lastDayOfMonth(month_start) + 1;
			else I = week_start* 7 + 1;

			for (i; i < I; i++) {
				c.setUnixFromDate(_dst_wt_year, month_start, i, h_start, 0, 0, getTimeZoneM());
				if (c.dayWeek() == dayWeek_start) {
					_dst_wt_day_Start = c.day();
					break;
				}
			}
			//end			
			if (!week_end) i = lastDayOfMonth(month_end) - 6;
			else i = (week_end - 1) * 7 + 1;
			
			if (!week_end) I = lastDayOfMonth(month_end) + 1;
			else I = week_end* 7 + 1;

			for (i; i < I; i++) {
				c.setUnixFromDate(_dst_wt_year, month_end, i, h_end, 0, 0, getTimeZoneM() + (60 * (dst_or_wt ? 1 : -1)));
				if (c.dayWeek() == dayWeek_end) {
					_dst_wt_day_End = c.day();
					break;
				}
			}

			if (unix() >= UNIXtime(_dst_wt_year, month_start, _dst_wt_day_Start, h_start, 0, 0, getTimeZoneM()).unix() &&
				unix() < UNIXtime(_dst_wt_year, month_end, _dst_wt_day_End, h_end, 0, 0, getTimeZoneM() + (60 * (dst_or_wt ? 1 : -1))).unix()) {
				if (dst_or_wt) setSummerTime();
				else setWinterTime();
				return 1;
			}
		}

		if (onDate(_dst_wt_day_Start, month_start, _dst_wt_year) && onTime(h_start, 0, 0) && getDST() == 0){
			if (dst_or_wt) setSummerTime();
			else setWinterTime();
			return 1;
		}
		if (onDate(_dst_wt_day_End, month_end, _dst_wt_year) && onTime(h_end, 0, 0) && getDST() == (dst_or_wt ? 1 : -1)) {
			setStandardTime();
			return 1;
		}
		
		return 0;
	}

    // мілісекунд з останнього оновлення
    uint32_t msFromUpdate() {
		if (_time) {
			// захист від переповнення різниці через 50 діб
			uint32_t diff = millis() - _last_upd;
			if (diff > 129600000ul) {	//1,5 доби
				_unix += diff / 1000ul;
				_last_upd = millis() - diff % 1000ul;
			}
			return diff;
		} else {
			return _ms;
		}
    }

    // unix час відносно грінвіча
    uint32_t unixGMT() {
        return _unix + (msFromUpdate() / 1000ul);
    }
	
	// unix час відносно даного time zone
    uint32_t unix() {
        return unixGMT() + _tz * 60L + (_dst_wt * 3600L);
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
        return (unix() % 3600ul) / 60ul;
    }

    // отримати години
    uint8_t hour() {
        return (unix() % 86400ul) / 3600ul;
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
        uint8_t _dayw = ((unix() / 86400ul) + 4) % 7;
        if (!_dayw) _dayw = 7;
        return _dayw;
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
	
	//
	bool everyH(uint8_t time_last, uint8_t time_out) {
		int8_t a = hour();
		if (time_out == 24 && time_last == a) return true;
		a -= time_last;
		if (a < 0) a = -a;
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool everyM(uint8_t time_last, uint8_t time_out) {
		int8_t a = minute();
		if (time_out == 60 && time_last == a) return true;
		a -= time_last;
		if (a < 0) a = -a;
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool everyS(uint8_t time_last, uint8_t time_out) {
		int8_t a = second();
		if (time_out == 60 && time_last == a) return true;
		a -= time_last;
		if (a < 0) a = -a;
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool everyMs(uint16_t time_last, uint16_t time_out) {
		int16_t a = ms();
		if (time_out == 1000 && time_last == a) return true;
		a -= time_last;
		if (a < 0) a = -a;
		if (a >= time_out) return true;
		return false;
	}
	
	//
	uint32_t periodInSec(uint32_t last_unix) {
		return unix() - last_unix;
	}
	
	//
	uint16_t periodInDays(uint32_t last_unix) {
		return periodInSec(last_unix) / 86400UL;
	}
	
	//
	bool timeOutD(uint32_t last_unix, uint16_t time_out) {
		uint16_t a = periodInDays(last_unix);
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool timeOutH(uint32_t last_unix, uint16_t time_out) {
		uint16_t a = periodInSec(last_unix);
		time_out = time_out * 3600;
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool timeOutM(uint32_t last_unix, uint16_t time_out) {
		uint16_t a = periodInSec(last_unix);
		time_out = time_out * 60;
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool timeOutS(uint32_t last_unix, uint16_t time_out) {
		uint16_t a = periodInSec(last_unix);
		if (a >= time_out) return true;
		return false;
	}
	
	bool isLeap(uint16_t y = 0) {
		if (!y) y = year();
		return  y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
	}
	
	uint8_t lastDayOfMonth() {
		lastDayOfMonth(month(), year());
	}
	
	uint8_t lastDayOfMonth(uint8_t m, uint16_t y = 0) {
		if (!m) m = month();
		if (!y) y = year();
		
		return m != 2 || !isLeap(y) ? pgm_read_byte(&UNIX_LAST_DAY_OF_MONTH[m-1]) : 29u;
	}
	
    // отримати рядок часу формата ГГ:ХХ:СС
    String timeString() {
        String str;
        str.reserve(8);
        if (hour() < 10) str += '0';
        str += hour();
        str += ':';
        if (minute() < 10) str += '0';
        str += minute();
        str += ':';
        if (second() < 10) str += '0';
        str += second();
        return str;
    }

    // отримати рядок дати формата ДД.ММ.РРРР
    String dateString() {
        String str;
        str.reserve(10);
		decodeTime();
        if (_day < 10) str += '0';
        str += _day;
        str += '.';
        if (_month < 10) str += '0';
        str += _month;
        str += '.';
        str += _year;
        return str;
    }
	
	//========================================================================
    String monthString(uint8_t m = 0) {
		if (!m) m = month();
        String str = (const char*)pgm_read_byte(&UNIX_MONTH_NAMES_EN[m-1]);
        return str;
    }
	
	//
    String dayWeekString(uint8_t wd = 0) {
		if (!wd) wd = dayWeek();
        String str = (const char*)pgm_read_byte(&UNIX_DAY_NAMES_EN[wd-1]);
        return str;
    }
	
	//
    String monthShortString(uint8_t m = 0) {
		if (!m) m = month();
        String str = (const char*)pgm_read_byte(&UNIX_MONTH_SHORT_NAMES_EN[m-1]);
        return str;
    }
	
	//
    String dayWeekShortString(uint8_t wd = 0) {
		if (!wd) wd = dayWeek();
        String str = (const char*)pgm_read_byte(&UNIX_DAY_SHORT_NAMES_EN[wd-1]);
        return str;
    }
	
	//=========================================================================
    String monthStringUA(uint8_t m = 0) {
		if (!m) m = month();
        String str = (const char*)pgm_read_byte(&UNIX_MONTH_NAMES_UA[m-1]);
        return str;
    }
	
	//
    String dayWeekStringUA(uint8_t wd = 0) {
		if (!wd) wd = dayWeek();
        String str = (const char*)pgm_read_byte(&UNIX_DAY_NAMES_UA[wd-1]);
        return str;
    }
	
	//
    String monthShortStringUA(uint8_t m = 0) {
		if (!m) m = month();
        String str = (const char*)pgm_read_byte(&UNIX_MONTH_SHORT_NAMES_UA[m-1]);
        return str;
    }
	
	//
    String dayWeekShortStringUA(uint8_t wd = 0) {
		if (!wd) wd = dayWeek();
        String str = (const char*)pgm_read_byte(&UNIX_DAY_SHORT_NAMES_UA[wd-1]);
        return str;
    }//=========================================================================
	
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