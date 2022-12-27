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
	bool isAM(uint8_t h) {
		return !isPM(h);  
	}
	
	//
	bool isPM(uint8_t h) {
		return (h >= 12); 
	}
	
	//
	uint8_t hourFormat12(uint8_t h) {
		if(h == 0 ) return 12;
		else if(h > 12) return h - 12 ;
		else return h ;
	}
	
	unix_t nextDays(unix_t unix, unix_t n) {
		unix += (unix_t)86400UL * n;
	}
	unix_t nextDay(unix_t unix) {
		unix += 86400UL;
	}
	
	unix_t prevDays(unix_t unix, unix_t n) {
		unix -= (unix_t)86400UL * n;
	}
	unix_t prevDay(unix_t unix) {
		unix -= 86400UL;
	}
	
	//
	bool everyH(uint8_t time_now, uint8_t time_last, uint8_t time_out) {
		int8_t a = time_now;
		if (time_out == 24 && time_last == a) return true;
		a -= time_last;
		if (a < 0) a = -a;
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool everyM(uint8_t time_now, uint8_t time_last, uint8_t time_out) {
		return everyS(time_now, time_last, time_out);
	}
	
	//
	bool everyS(uint8_t time_now, uint8_t time_last, uint8_t time_out) {
		int8_t a = time_now;
		if (time_out == 60 && time_last == a) return true;
		a -= time_last;
		if (a < 0) a = -a;
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool everyMs(uint16_t time_now, uint16_t time_last, uint16_t time_out) {
		int16_t a = time_now;
		if (time_out == 1000 && time_last == a) return true;
		a -= time_last;
		if (a < 0) a = -a;
		if (a >= time_out) return true;
		return false;
	}
	
	//
	unix_t periodInSec(unix_t unix_now, unix_t last_unix) {
		return unix_now - last_unix;
	}
	
	//
	unix_t periodInFullDays(unix_t unix_now, unix_t last_unix) {
		return periodInSec(unix_now, last_unix) / 86400UL;
	}
	
	//
	unix_t periodInDays(unix_t unix_now, unix_t last_unix) {
		return (unix_now / 86400UL) - (last_unix / 86400UL);
	}
	
	//
	unix_t periodInMonths(unix_t unix_now, unix_t last_unix) {
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
	bool timeOutMonth(unix_t unix_now, unix_t last_unix, unix_t time_out) {
		uint32_t a = periodInMonths(unix_now, last_unix);
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool timeOutD(unix_t unix_now, unix_t last_unix, unix_t time_out) {
		uint32_t a = periodInDays(unix_now, last_unix);
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool timeOutFullDays(unix_t unix_now, unix_t last_unix, unix_t time_out) {
		uint32_t a = periodInFullDays(unix_now, last_unix);
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool timeOutH(unix_t unix_now, unix_t last_unix, unix_t time_out) {
		unix_t a = periodInSec(unix_now, last_unix);
		time_out = time_out * 3600;
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool timeOutM(unix_t unix_now, unix_t last_unix, unix_t time_out) {
		unix_t a = periodInSec(unix_now, last_unix);
		time_out = time_out * 60;
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool timeOutS(unix_t unix_now, unix_t last_unix, unix_t time_out) {
		unix_t a = periodInSec(unix_now, last_unix);
		if (a >= time_out) return true;
		return false;
	}
	
	bool isLeap(year_t y) {
		return  y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
	}
	
	uint8_t lastDayOfMonth(uint8_t m, year_t y) {
		return m != 2 || !isLeap(y) ? pgm_read_byte(&UNIX_LAST_DAY_OF_MONTH[m-1]) : 29u;
	}
	
	unix_t unixGMTFromDate(year_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM) {
		int8_t my = (m >= 3) ? 1 : 0;
		y += my - 1970;
		uint16_t dm = 0;
		for (int i = 0; i < m - 1; i++) dm += (i<7)?((i==1)?28:((i&1)?30:31)):((i&1)?31:30);
		return (((d-1+dm+((y+1)>>2)-((y+69)/100)+((y+369)/100/4)+365*(y-my))*24ul+h-(tzM/60))*60ul+mn-(tzM%60))*60ul+s;
	}
	
	void decodeUNIX(unix_t unix, uint8_t& d, uint8_t& m, year_t& y) {
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
	
	void decodeUNIX(unix_t unix, uint8_t& d, uint8_t& m, year_t& y, uint8_t& h, uint8_t& min, uint8_t& s) {
        s = unix % 60;
        min = (unix % 3600UL) / 60UL;
        h = (unix % 86400UL) / 3600UL;
		decodeUNIX(unix, d, m, y);
	}
	
	uint8_t dayWeek(unix_t unix) {
		uint8_t _dayw = ((unix / 86400UL) + 4) % 7;
        if (!_dayw) _dayw = 7;
        return _dayw;
	}
	
	void delay(uint32_t t, void (*func)()) {
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
	
    // отримати рядок часу формата ГГ:ХХ:СС
    String timeString(uint8_t h, uint8_t m, uint8_t s) {
        String str;
        str.reserve(8);
        if (h < 10) str += '0';
        str += h;
        str += ':';
        if (m < 10) str += '0';
        str += m;
        str += ':';
        if (s < 10) str += '0';
        str += s;
        return str;
    }

    // отримати рядок дати формата ДД.ММ.РРРР
    String dateString(uint8_t d, uint8_t m, year_t y) {
        String str;
        str.reserve(10);
        if (d < 10) str += '0';
        str += d;
        str += '.';
        if (m < 10) str += '0';
        str += m;
        str += '.';
        str += y;
        return str;
    }
	
	//========================================================================
    String monthString(uint8_t m) {
        String str = (const char*)pgm_read_byte(&UNIX_MONTH_NAMES_EN[m-1]);
        return str;
    }
	
	//
    String dayWeekString(uint8_t wd) {
        String str = (const char*)pgm_read_byte(&UNIX_DAY_NAMES_EN[wd-1]);
        return str;
    }
	
	//
    String monthShortString(uint8_t m) {
        String str = (const char*)pgm_read_byte(&UNIX_MONTH_SHORT_NAMES_EN[m-1]);
        return str;
    }
	
	//
    String dayWeekShortString(uint8_t wd) {
        String str = (const char*)pgm_read_byte(&UNIX_DAY_SHORT_NAMES_EN[wd-1]);
        return str;
    }
	
	//=========================================================================
    String monthStringUA(uint8_t m) {
        String str = (const char*)pgm_read_byte(&UNIX_MONTH_NAMES_UA[m-1]);
        return str;
    }
	
	//
    String dayWeekStringUA(uint8_t wd) {
        String str = (const char*)pgm_read_byte(&UNIX_DAY_NAMES_UA[wd-1]);
        return str;
    }
	
	//
    String monthShortStringUA(uint8_t m) {
        String str = (const char*)pgm_read_byte(&UNIX_MONTH_SHORT_NAMES_UA[m-1]);
        return str;
    }
	
	//
    String dayWeekShortStringUA(uint8_t wd) {
        String str = (const char*)pgm_read_byte(&UNIX_DAY_SHORT_NAMES_UA[wd-1]);
        return str;
    }
	
	//=========================================================================
    String monthStringRU(uint8_t m) {
        String str = (const char*)pgm_read_byte(&UNIX_MONTH_NAMES_RU[m-1]);
        return str;
    }
	
	//
    String dayWeekStringRU(uint8_t wd) {
        String str = (const char*)pgm_read_byte(&UNIX_DAY_NAMES_RU[wd-1]);
        return str;
    }
	
	//
    String monthShortStringRU(uint8_t m) {
        String str = (const char*)pgm_read_byte(&UNIX_MONTH_SHORT_NAMES_RU[m-1]);
        return str;
    }
	
	//
    String dayWeekShortStringRU(uint8_t wd) {
        String str = (const char*)pgm_read_byte(&UNIX_DAY_SHORT_NAMES_RU[wd-1]);
        return str;
    }//=========================================================================
	
};

static TimeFunctions TimeFunc;
#endif