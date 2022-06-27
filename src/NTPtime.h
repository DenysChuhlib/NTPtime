/*

Денис Чухліб ,https://github.com/DenysChuhlib
MIT License

*/

#ifndef NTPtime_h
#define NTPtime_h

#include "UNIXtime.h"

#define _NTPtime_NTP_TIMEOUT 64000
#define _NTPtime_NTP_PORT 123

#define NTP_OK 0
#define NTP_NOT_STARTED 1
#define NTP_NOT_CONNECTED_WIFI 2
#define NTP_NOT_CONNECTED_TO_SERVER 3
#define NTP_NOT_SENT_PACKET 4
#define NTP_WAITING_REPLY 5
#define NTP_TIMEOUT 6
#define NTP_REPLY_ERROR 7

const PROGMEM char* _NTPtime_DEFAULT_HOST = "pool.ntp.org";

#ifndef NTPtimeEthernetUdp
#include <WiFiUdp.h>
#else
#include <EthernetUdp.h>
#define NTP_NOT_CONNECTED_Ethernet 2
#endif

/*example
uint8_t Clock_strata;
#define NTPtimeClockStrata_val Clock_strata
*/
//https://en.wikipedia.org/wiki/Network_Time_Protocol#Clock_strata


class NTPtime : public UNIXtime {
public:
	NTPtime(int8_t gmt = 0, int8_t gmtM = 0) {
        setTimeZone(gmt, gmtM);
		_time = true;
		_time_stat = 1;
    }
	
	~NTPtime() {
		end();
    }

    // установити хост (за умвч. pool.ntp.org)
    void setHost(const char* host) {
        _host = host;
    }

    // запустити
    bool begin(uint16_t port = _NTPtime_NTP_PORT) {
        _ntp_stat = !udp.begin(port);
		return !_ntp_stat;
    }

    // остановити
    void end() {
        udp.stop();
        _ntp_stat = 1;
    }
	
	// асинхронний режим (за умовч. включений, true)
    void asyncMode(bool _on_off) {
        _async = _on_off;
    }
	
	//обновити зараз
	void updateNow() {
		setTimeStat(UNIX_NOT_SYNCHRONIZED);
	}
	
	// обновити час в Г, Х, С
	bool updateOnTime(uint8_t h, uint8_t m, uint8_t s) {
        if (onTime(h, m ,s) && msFromUpdate() >= 5000) {
			updateNow();
			return 1;
		}
		return 0;
    }

	// ============== TICK ===============
    // обновляє час по власному таймеру
    bool tick(uint16_t prd = 0) {
		if (prd >= 60 && prd <= 86400 && msFromUpdate() >= prd  * 1000) setTimeStat(UNIX_NOT_SYNCHRONIZED);
		
		if (_ntp_stat != NTP_NOT_STARTED && _time_stat == UNIX_NOT_SYNCHRONIZED) {
			do {
				_ntp_stat = requestTime();				// запит NTP
				
				if (!_async) yield();
			} while (!_async && _ntp_stat == NTP_WAITING_REPLY);
			
			return 1;
		} else if (_time_stat == UNIX_NOT_STARTED || _ntp_stat == NTP_NOT_STARTED) {
			if (_send_pack) _send_pack = false;
		}
        return 0;
    }

    // отримати пінг сервера
    int16_t ping() {
        return _ping;
    }

    // отримати статус оновлення
    /*
        NTP_OK 							0 - все ок
        NTP_NOT_STARTED 				1 - не запущений UDP
        NTP_NOT_CONNECTED_WIFI 			2 - не підключений WiFi
        NTP_NOT_CONNECTED_TO_SERVER 	3 - помилка підключення до сервера
        NTP_NOT_SENT_PACKET 			4 - помилка отправки пакета
		NTP_WAITING_REPLY 				5 - іде очікування відповіді
        NTP_TIMEOUT 					6 - таймаут відповіді сервера
        NTP_REPLY_ERROR 				7 - отримана некоректна відповідь сервера
    */
    uint8_t NTPstatus() {
        return _ntp_stat;
    }
	
	// отримати статус системи
    /*
        UNIX_OK 				0 - все ок
        UNIX_NOT_SYNCHRONIZED 	1 - не синхронізовано
        UNIX_NOT_STARTED 		2 - зупинено
    */
	/*bool status() { 			//UNIXtime.h
        return _time_stat;
    }*/

private:
	// запитати і обновити час з сервера
    uint8_t requestTime() {
		uint8_t buf[48];
		#ifndef NTPtimeEthernetUdp
		if (!WiFi.isConnected()) return 2;
		#else
		if (Ethernet.linkStatus() == LinkOFF || Ethernet.hardwareStatus() == EthernetNoHardware) return 2;
		#endif
		if (!_send_pack) {//перевіряємо чи не був відправлений запит
			memset(buf, 0, 48);
			//https://en.wikipedia.org/wiki/Network_Time_Protocol, https://ru.wikipedia.org/wiki/NTP
			buf[0] = 0b11100011;                    // LI 0x3, v4, client
			buf[2] = 6; 							//таймаут 2^6 = 64 секунд
			buf[3] = 0xEC; 							//точність -20 - двійковий логарифм секунд
			if (!udp.beginPacket(_host, _NTPtime_NTP_PORT)) return 3;
			udp.write(buf, 48);
			if (!udp.endPacket()) return 4;
			_way = millis();
			_send_pack = true;
		}
		if (_send_pack) {//перевіряємо чи був ли відправлений запит
			if (udp.parsePacket() != 48 || udp.remotePort() != _NTPtime_NTP_PORT) {
				if (millis() - _way > _NTPtime_NTP_TIMEOUT) {
					_send_pack = false;
					return 6;
				}
			} else {
				udp.read(buf, 48);             // читаємо
				if (buf[40] == 0 || buf[40] < 98) {  	// некоректний час
					_send_pack = false;
					return 7;
				}
				_last_upd = millis();                   // запам'ятали час оновлення
				uint16_t a_ms = ((buf[44] << 8) | buf[45]) * 1000L >> 16;						// мс сервера
				int16_t ser_del = (int16_t)a_ms - (((buf[36] << 8) | buf[37]) * 1000L >> 16);	// мс затримки сервера
				if (ser_del < 0) ser_del += 1000;   // перехід через секунду
				_way = millis() - _way;				// весь шлях пакета і затримка сервера (фактично це пінг, але пакет NTP більший ніж пакет ping і тому затримка сервера більша)
				_ping = _way - ser_del;				// нинішній пінг (шлях пакета, або RTT)
				_way = _ping / 2;					// середній шлях в одну сторону
				_last_upd -= (a_ms + _way);      	// затримка часу
				_unix = (uint32_t)(buf[40] << 8 | buf[41]) << 16 | (buf[42] << 8 | buf[43]); // 1900
				_unix -= 2208988800ul;              // переводимо в UNIX (1970)
				_send_pack = false;
				setTimeStat(UNIX_OK);
				#ifdef NTPtimeClockStrata_val
					NTPtimeClockStrata_val = buf[1];
				#endif
				return 0;
			}
		}
		return 5;
    }
	
	#ifndef NTPtimeEthernetUdp
	WiFiUDP udp;
	#else
	EthernetUDP udp;
	#endif
    const char* _host = _NTPtime_DEFAULT_HOST;

	bool _async : 1 = 1;
	bool _send_pack : 1 = 0;
	uint8_t _ntp_stat : 3 = 1;
	uint32_t _way = 0;
	int16_t _ping = -1;
	
	//bool _time = 0;
	//uint8_t _time_stat = 2;
	
	//uint32_t _last_upd = 0;
    //uint32_t _unix = 0;
};
#endif