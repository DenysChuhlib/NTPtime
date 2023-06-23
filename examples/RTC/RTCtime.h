#ifndef RTCtime_H
#define RTCtime_H

class RTCtime : public UNIXtime {
public:
	RTCtime(int8_t gmt = 0, int8_t gmtM = 0) {
        setTimeZone(gmt, gmtM);
		startTime();
    }
	
	bool begin(); /*{
		// сюди вставити функцію для запуску модуля RTC
	}*/
	
	void setUnixGMT_RTC(uint32_t unix); /*{
		// сюди вставити функцію для встановлення часу в RTC
	}*/

	unix_t getUnixGMT_RTC(); /*{
		// сюди вставити функцію для отримання часу з RTC
	}*/

	
	//обновити зараз
	void updateNow() {
		setTimeStat(UNIX_NOT_SYNCHRONIZED);
	}
	
	// обновити час в Г, Х, С
	bool updateOnTime(uint8_t h, uint8_t m, uint8_t s) {
        if (onTime(h, m ,s) && msFromUpdate() >= 5000UL) {
			updateNow();
			return 1;
		}
		return 0;
    }
	
	
	void tick(uint32_t prd = 0) {
		if (prd) {
			prd = constrain(prd, 60, 86400UL);
			if (msFromUpdate() >= prd  * 1000UL) setTimeStat(UNIX_NOT_SYNCHRONIZED);
		} else msFromUpdate();// перевірка переповнення
		
		if (status() == UNIX_NOT_SYNCHRONIZED) {
			unix = getUnixGMT_RTC();
			_ms = millis();
			setTimeStat(UNIX_OK);
			return 1;
		}
		return 0;
	}
};

#endif