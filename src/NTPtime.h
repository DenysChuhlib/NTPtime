/*

Денис Чухліб ,https://github.com/DenysChuhlib
MIT License

*/

#ifndef NTPtime_h
#define NTPtime_h

#include "UNIXtime.h"

#define NTP_OK 0
#define NTP_NOT_STARTED 1
#define NTP_NOT_CONNECTED_WIFI 2
#define NTP_NOT_CONNECTED_TO_SERVER 3
#define NTP_NOT_SENT_PACKET 4
#define NTP_WAITING_REPLY 5
#define NTP_TIMEOUT 6
#define NTP_REPLY_ERROR 7

const PROGMEM char* _NTPtime_DEFAULT_HOST = "pool.ntp.org";

#ifndef NTPtimeEthernet
//
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
//
#include <WiFiUdp.h>
#else//
#include <Ethernet.h>
#include <EthernetUdp.h>
#define NTP_NOT_CONNECTED_Ethernet 2
#endif//

/*example
uint8_t Clock_strata;
#define NTPtimeClockStrata_val Clock_strata
*/
//https://en.wikipedia.org/wiki/Network_Time_Protocol#Clock_strata


class NTPtime : public UNIXtime {
public:
	NTPtime(int8_t gmt = 0, int8_t gmtM = 0) {
        setTimeZone(gmt, gmtM);
		startTime();
    }
	
	~NTPtime() {
		end();
    }

    // установити хост (за умвч. pool.ntp.org)
    void setHost(const String& host) {
        _host = host;
    }
	
	void setPort(uint16_t port) {
		_port = port;
	}
	
	void setTimeout(uint16_t timeout) {
		_timeout = constrain(timeout, 200, 64000);
	}

    // запустити
    bool begin(uint16_t port = 123) {
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
        if (onTime(h, m ,s) && msFromUpdate() >= 5000UL) {
			updateNow();
			return 1;
		}
		return 0;
    }

	// ============== TICK ===============
    // обновляє час по власному таймеру
    bool tick(uint32_t prd = 0) {
		if (prd) {
			prd = constrain(prd, 60, 86400UL);
			if (msFromUpdate() >= prd  * 1000UL) setTimeStat(UNIX_NOT_SYNCHRONIZED);
		} else msFromUpdate();// перевірка переповнення
		
		if (_ntp_stat != NTP_NOT_STARTED && status() == UNIX_NOT_SYNCHRONIZED) {
			do {
				_ntp_stat = requestTime();				// запит NTP
				
				if (!_async) yield();
			} while (!_async && _ntp_stat == NTP_WAITING_REPLY);
			
			return 1;
		} else if (status() == UNIX_NOT_STARTED || _ntp_stat == NTP_NOT_STARTED) {
			_send_pack = false;
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
	/*bool status();*/ 			//UNIXtime.h

private:
	// запитати і обновити час з сервера
    uint8_t requestTime() {
		uint8_t buf[48];
		#ifndef NTPtimeEthernet
		if (!WiFi.isConnected()) return 2;
		#else
		if (Ethernet.linkStatus() == LinkOFF || Ethernet.hardwareStatus() == EthernetNoHardware) return 2;
		#endif
		if (!_send_pack) {//перевіряємо чи не був відправлений запит
			memset(buf, 0, 48);
			//https://en.wikipedia.org/wiki/Network_Time_Protocol, https://ru.wikipedia.org/wiki/NTP
			buf[0] = 0b11100011;                    // LI 0x3, v4, client
			buf[2] = 6; 							//таймаут 2^6 = 64 секунд
			if (!udp.beginPacket(_host.c_str(), _port)) return 3;
			udp.write(buf, 48);
			if (!udp.endPacket()) return 4;
			_way = millis();
			_send_pack = true;
		}
		if (_send_pack) {//перевіряємо чи був ли відправлений запит
			if (udp.parsePacket() != 48 || udp.remotePort() != _port) {
				if (millis() - _way > (uint32_t)_timeout) {
					_send_pack = false;
					_ping = -1;
					return 6;
				}
			} else {
				udp.read(buf, 48);             		// читаємо
				{
					uint32_t got_time = millis();       // запам'ятали час оновлення
					uint16_t serv_ms = ((buf[44] << 8) | buf[45]) * 1000L >> 16;						// мс сервера
					int16_t ser_del = (int16_t)serv_ms - (((buf[36] << 8) | buf[37]) * 1000L >> 16);	// мс затримки сервера
					if (ser_del < 0) ser_del += 1000;   // перехід через секунду
					_way = millis() - _way;				// весь шлях пакета і затримка сервера (фактично це пінг, але пакет NTP більший ніж пакет ping і тому затримка сервера більша)
					int16_t ping = _way - ser_del;		// нинішній пінг (шлях пакета, або RTT)
					_way = ping / 2;					// середній шлях в одну сторону
					got_time -= (serv_ms + _way);      	// затримка часу
					unix_t unix = (uint32_t)(buf[40] << 8 | buf[41]) << 16 | (buf[42] << 8 | buf[43]); // 1900
					unix -= 2208988800UL;              // переводимо в UNIX (1970)
					
					if (unix < 1662757200ul) {  		// некоректний час
						_send_pack = false;
						_ping = -1;
						return 7;
					}
					
					_ping = ping;
					_ms = got_time;
					_unix = unix;
				}
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
	
	#ifndef NTPtimeEthernet
	WiFiUDP udp;
	#else
	EthernetUDP udp;
	#endif
    String _host = FPSTR(_NTPtime_DEFAULT_HOST);
	uint16_t _port = 123;
	uint16_t _timeout = 64000;

	bool _async : 1 = 1;
	bool _send_pack : 1 = 0;
	uint8_t _ntp_stat : 3 = 1;
	uint32_t _way = 0;
	int16_t _ping = -1;
	
	//uint32_t _ms = 0;
    //unix_t _unix = 0;
};
#endif