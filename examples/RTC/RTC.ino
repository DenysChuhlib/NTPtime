#include <UNIXtime.h>

//підключити бібліотеку RTC
//створити об'єкт
#include <Wire.h>
#include <RTClib.h>
RTC_DS1307 RTC;

#include "RTCtime.h"

bool RTCtime::begin() {
	// сюди вставити функцію для запуску модуля RTC
	return RTC.begin();
}

void RTCtime::setUnixGMT_RTC(uint32_t unix) {
	// сюди вставити функцію для встановлення часу в RTC
	RTC.adjust(DateTime(unix));
}

unix_t RTCtime::getUnixGMT_RTC() {
	// сюди вставити функцію для отримання часу з RTC
	return RTC.now().unixtime();
}

RTCtime Time;

void setup() {
	Time.begin();
	
	if (!RTC.isrunning()) {
		Serial.println("RTC is NOT running, let's set the time!");
		// When time needs to be set on a new device, or after a power loss, the
		// following line sets the RTC to the date & time this sketch was compiled
		RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
		// This line sets the RTC with an explicit date & time, for example to set
		// January 21, 2014 at 3am you would call:
		// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
	}
}

void loop() {
	Time.tick();
	if(Time.everyMs(0,1000)) {
		Serial.println(Time.timeString());
	}
}