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

struct TimeFunctions {
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
	uint32_t periodInSec(uint32_t unix_now, uint32_t last_unix) {
		return unix_now - last_unix;
	}
	
	//
	uint16_t periodInDays(uint32_t unix_now, uint32_t last_unix) {
		return periodInSec(unix_now, last_unix) / 86400UL;
	}
	
	//
	uint16_t periodInMonths(uint32_t unix_now, uint32_t last_unix) {
		uint16_t year;
		uint8_t month, day;
		uint16_t year_last;
		uint8_t month_last, day_last;
		for (uint8_t i = 0; i < 2; i++) {// http://howardhinnant.github.io/date_algorithms.html#civil_from_days
			yield();
			uint32_t u;
			if (i == 0) u = unix_now / 86400ul;
			else u = last_unix / 86400ul;
			u += 719468;
			uint8_t era = u / 146097ul;
			uint16_t doe = u - era * 146097ul;
			uint16_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
			if (i == 0) year = yoe + era * 400;
			else year_last = yoe + era * 400;
			uint16_t doy = doe - (yoe * 365 + yoe / 4 - yoe / 100);
			uint16_t mp = (doy * 5 + 2) / 153;
			if (i == 0) {
				day = doy - (mp * 153 + 2) / 5 + 1;
				month = mp + (mp < 10 ? 3 : -9);
				year += (month <= 2);
			} else {
				day_last = doy - (mp * 153 + 2) / 5 + 1;
				month_last = mp + (mp < 10 ? 3 : -9);
				year_last += (month_last <= 2);
			}
		}
		uint16_t p;
	
		p = (year - year_last) * 12;
		p += month - month_last;
		if (day < day_last) {
			p--;
		}
		return p;
	}
	
	//========================================================================
	bool timeOutMonth(uint32_t unix_now, uint32_t last_unix, uint16_t time_out) {
		uint16_t a = periodInMonths(unix_now, last_unix);
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool timeOutD(uint32_t unix_now, uint32_t last_unix, uint16_t time_out) {
		uint16_t a = periodInDays(unix_now, last_unix);
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool timeOutH(uint32_t unix_now, uint32_t last_unix, uint16_t time_out) {
		uint16_t a = periodInSec(unix_now, last_unix);
		time_out = time_out * 3600;
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool timeOutM(uint32_t unix_now, uint32_t last_unix, uint16_t time_out) {
		uint16_t a = periodInSec(unix_now, last_unix);
		time_out = time_out * 60;
		if (a >= time_out) return true;
		return false;
	}
	
	//
	bool timeOutS(uint32_t unix_now, uint32_t last_unix, uint16_t time_out) {
		uint16_t a = periodInSec(unix_now, last_unix);
		if (a >= time_out) return true;
		return false;
	}
	
	bool isLeap(uint16_t y) {
		return  y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
	}
	
	uint8_t lastDayOfMonth(uint8_t m, uint16_t y) {
		return m != 2 || !isLeap(y) ? pgm_read_byte(&UNIX_LAST_DAY_OF_MONTH[m-1]) : 29u;
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
    String dateString(uint8_t d, uint8_t m, uint8_t y) {
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