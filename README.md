[![Foo](https://img.shields.io/badge/Library%20Manager-NTPtime%201.1.2-000282.svg?style=for-the-badge&logo=arduino)](https://www.ardu-badge.com/NTPtime)

[![Foo](https://img.shields.io/badge/README-English-fff700.svg?style=for-the-badge)](https://github-com.translate.goog/DenysChuhlib/NTPtime?_x_tr_sl=uk&_x_tr_tl=en)
[![Foo](https://img.shields.io/badge/README-Руский-fff700.svg?style=for-the-badge)](https://github-com.translate.goog/DenysChuhlib/NTPtime?_x_tr_sl=uk&_x_tr_tl=ru)

# NTPtime
Бібліотека часу з багатьма функціями:
- Отримання точного часу за допомогою NTP сервера і використання
- Використання UNIX часу і його декодування
- Альтернатива стандартній бібліотеці `Time.h` в оптимізації і простоті використання
- Можливість робити таймер у рівний час, наприклад рівно о 12:00 і так щогодини, який займає лише один байт
- Безліч інших зручних функцій
- Можна робити цілий календар

### Сумісність усієї бібліотеки
Будь-які ардуіно сумісні плати

### Сумісність синхронізації NTP
esp8266, esp32, Ethernet

## Зміст
- [Установка](#install)
- [Ініціалізація](#init)
- [Використання](#usage)
    - [NTPtime](#NTPtime)
    - [UNIXtime і NTPtime](#UNIXtime&NTPtime)
    - [TimeFunc](#TimeFunc)
- [Приклад](#example)
- [Версії](#versions)

<a id="install"></a>
## Установка
- Бібліотеку можна знайти за назвою `NTPtime` та встановити через менеджер бібліотек у:
    - Arduino IDE
    - Arduino IDE v2.0
- [Завантажити бібліотеку](https://github.com/DenysChuhlib/NTPtime/archive/refs/heads/main.zip) .zip архівом для ручної установки:
    - Розпакувати та покласти в `Документи/Arduino/libraries/`
    - (Arduino IDE) автоматичне встановлення з .zip: `Скетч/Підключити бібліотеку/Додати .ZIP бібліотеку…` та вказати завантажений архів

<a id="init"></a>
## Ініціалізація
```cpp
NTPtime Time; 							// параметри за замовчуванням (time zone 0, time zone minune)
NTPtime(time_zona, time_zonaM); 				// часовий пояс у годинах і хвилинах

UNIXtime Timer; 						// таймер (запустити Timer.startTime();)
UNIXtime(uint32_t unix, int8_t tz, int8_t tzM, uint16_t ms) //unix час, часовий пояс, хвилини часового поясу, міліскекунди
UNIXtime(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM, uint16_t ms) // рік, місяць, день, години, хвилини, секунди, часовий пояс у хвилинах, мілісекунди
````
<a id="usage"></a>
## Використання
<a id="NTPtime"></a>
### NTPtime
```cpp

void setHost(const String& host); 				// установити хост (за умовч. pool.ntp.org)
void setPort(uint16_t port);					// установити порт NTP сервера (за умовч. 123)
void setTimeout(uint16_t timeout);				// установити таймаут відповіді сервера (200 - 64000) (за умовч. 64000)

bool begin(uint16_t port); 				// запустити (вхідний порт за умовч. 123)
void end(); 							// зупинити

void asyncMode(bool _on_off); 					// асинхронний режим (за умовч. ввімкнений, true)

void updateNow(); 						// обновити зараз (ручний запуск оновлення)

bool updateOnTime(uint8_t h, uint8_t m, uint8_t s); 		// функція `loop()` цикла яка оновлює час в Г, Х, С (працює в парі з tick())

bool tick(uint16_t prd);					// функція `loop()` цикла яка оновлює час по періоду (1 хв <= prd <= 1 доба), після ручного запуска або в певний час, підчас оновлення повертає true

int16_t ping(); 						// отримати пінг сервера (при відсутньому з'єднанні повертає -1)

// отримати статус оновлення
/*
    NTP_OK 					0 - все ок
    NTP_NOT_STARTED 				1 - не запущений UDP
    NTP_NOT_CONNECTED_WIFI 			2 - не підключений WiFi
    NTP_NOT_CONNECTED_TO_SERVER 		3 - помилка підключення до сервера
    NTP_NOT_SENT_PACKET 			4 - помилка отправки пакета
    NTP_WAITING_REPLY 				5 - іде очікування відповіді
    NTP_TIMEOUT 				6 - таймаут відповіді сервера
    NTP_REPLY_ERROR 				7 - отримана некоректна відповідь сервера
*/
uint8_t NTPstatus(); 
```
### Особливості
- `tick()`, `DST()`, `updateOnTime()` працюють головному в циклі `loop()`
- Чим більше потрібна точність часу тим меньше потрібен період в `tick(prd)`. Якщо простий годинник достатньо одного updateOnTime(0,0,10) і обновляти в ніч, коли найкращий Інтернет

<a id="UNIXtime&NTPtime"></a>
### UNIXtime і NTPtime

```cpp
void setUnixGMT(uint32_t unix, uint16_t ms); 			// встановити unix час відносно грінвіча

void setUnixFromDate(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM = 0, uint16_t ms = 0);	// назва функції говорить сама за себе

void setTimeZone(int8_t tz, int8_t tzM); 			// установити часовий пояс
int16_t getTimeZoneM();						// дізнатись часовий пояс в хвилинах

void setDST (int8_t dst_wt); 					// установити літній час 1 (за умовчуванням) та зимовий час -1 або стандартний час 0
void setSummerTime(); 						// установити літній час

void setWinterTime(); 						// установити зимовий час

void setStandardTime();						// установити стандартний час

int8_t getDST(); 						// поверне якщо літній час 1 та зимовий час -1 або стандартний час 0

void DST(uint8_t month_start, uint8_t week_start, uint8_t dayWeek_start, uint8_t h_start,
uint8_t month_end, uint8_t week_end, uint8_t dayWeek_end, uint8_t h_end, bool dst_or_wt);
//month_start - місяць початку літнього/зимнього часу
//week_start  -  тиждень початку літнього/зимнього часу (якщо 0 то останній тиждень)
//dayWeek_start - день тижня початку літнього/зимнього часу
//h_start - година початку літнього/зимнього часу

//month_end - місяць кінця літнього/зимнього часу
//week_end - тиждень кінця літнього/зимнього часу (якщо 0 то останній тиждень)
//dayWeek_end - день тижня кінця літнього/зимнього часу
//h_end - година кінця літнього/зимнього часу

//dst_or_wt за умовчування 1 - літній час, якщо 0 то зимній
//DST(2, 0, 7, 3, 10, 0, 7, 4); //https://en.wikipedia.org/wiki/Eastern_European_Summer_Time

// отримати статус системи
/*
    UNIX_OK 			0 - все ок
    UNIX_NOT_SYNCHRONIZED 	1 - не синхронізовано
    UNIX_NOT_STARTED 		2 - зупинено
*/	
bool status();

void startTime();						// почати відлік часу

void stopTime();						// зупунити відлік часу

uint32_t msFromUpdate(); 					// мілісекунд з останнього оновлення

uint32_t unixGMT(); 						// unix час відносно грінвіча

uint32_t unix(); 						// unix час відносно даного часового пояса

uint16_t ms(); 							// мілісекунди

uint8_t second(); 						// отримати секунди

uint8_t minute(); 						// отримати хвилини

uint8_t hour(); 						// отримати години

uint8_t day(); 							// отримати день місяця

uint8_t month(); 						// отримати місяць

uint16_t year(); 						// отримати рік

uint8_t dayWeek(); 						// отримати день тижня

bool isAM();
bool isPM();
uint8_t hourFormat12();

bool onTime (uint8_t h, uint8_t m, uint8_t s);			// назва функції говорить сама за себе

bool onDate (uint8_t d, uint8_t m, uint8_t y);			// назва функції говорить сама за себе

bool timeAfter (uint8_t h, uint8_t m, uint8_t s);		// назва функції говорить сама за себе

bool dateAfter (uint8_t d, uint8_t m, uint8_t y);		// назва функції говорить сама за себе

bool everyH(uint8_t time_last, uint8_t time_out);		// прості і маленькі таймери (time_last - останній збережений hour()) (time_out від 0 до 24)
bool everyM(uint8_t time_last, uint8_t time_out);		// (time_last - останній збережений minute()) (time_out від 0 до 60)
bool everyS(uint8_t time_last, uint8_t time_out);		// (time_last - останній збережений second()) (time_out від 0 до 60)
bool everyMs(uint16_t time_last, uint16_t time_out);		// (time_last - останній збережений ms()) (time_out від 0 до 1000)

uint32_t periodInSec(uint32_t last_unix);			// період в секундах (last_unix - останній збережений unix())
uint16_t periodInDays(uint32_t last_unix);
uint16_t periodInFullDays(uint32_t last_unix);
uint16_t periodInMonths(uint32_t last_unix);

bool timeOutMonth(uint32_t last_unix, uint16_t time_out);	// якщо період більше або дорівнює таймауту то повертає true
bool timeOutD(uint32_t last_unix, uint16_t time_out);
bool timeOutH(uint32_t last_unix, uint16_t time_out);
bool timeOutM(uint32_t last_unix, uint16_t time_out);
bool timeOutS(uint32_t last_unix, uint16_t time_out);

bool isLeap();							// якщо високосний рік то повертає true

uint8_t lastDayOfMonth();					// останній день місяця

void delay(uint32_t t, void (*func)());		// з функцією яка виконуеться підчас затримки)

String timeString();						// отримати рядок часу формата ГГ:ХХ:СС

String dateString();						// отримати рядок дати формата ДД.ММ.РРРР

String monthString();
String dayWeekString();
String monthShortString();
String dayWeekShortString();

String monthStringUA();
String dayWeekStringUA();
String monthShortStringUA();
String dayWeekShortStringUA();

String monthStringRU();
String dayWeekStringRU();
String monthShortStringRU();
String dayWeekShortStringRU();
```
### Особливості
- Мілісекунди зберігаються при зупинці відліку часу і відновлюються при запуску.

<a id="TimeFunc"></a>
### TimeFunc
Просто
 
```cpp
 TimeFunc.isLeap(uint16_t y); 
```
 і все.

```cpp

bool isAM(uint8_t h);
bool isPM(uint8_t h);
uint8_t hourFormat12(uint8_t h);

bool everyH(uint8_t time_now, uint8_t time_last, uint8_t time_out);		// прості і маленькі таймери (time_last - останній збережений hour()) (time_out від 0 до 24)
bool everyM(uint8_t time_now, uint8_t time_last, uint8_t time_out);		// (time_last - останній збережена хвилина) (time_out від 0 до 60)
bool everyS(uint8_t time_now, uint8_t time_last, uint8_t time_out);		// (time_last - останній збережена секунда) (time_out від 0 до 60)
bool everyMs(uint16_t time_now, uint16_t time_last, uint16_t time_out);		// (time_last - останній збережена мілісекунда) (time_out від 0 до 1000)

uint32_t periodInSec(uint32_t unix_now, uint32_t last_unix);			// період в секундах (last_unix - останній збережений unix())
uint16_t periodInDays(uint32_t unix_now, uint32_t last_unix);
uint16_t periodInFullDays(uint32_t unix_now, uint32_t last_unix);
uint16_t periodInMonths(uint32_t unix_now, uint32_t last_unix);

bool timeOutMonth(uint32_t unix_now, uint32_t last_unix, uint16_t time_out);	// якщо період більше або дорівнює таймауту то повертає true
bool timeOutD(uint32_t unix_now, uint32_t last_unix, uint16_t time_out);
bool timeOutH(uint32_t unix_now, uint32_t last_unix, uint16_t time_out);
bool timeOutM(uint32_t unix_now, uint32_t last_unix, uint16_t time_out);
bool timeOutS(uint32_t unix_now, uint32_t last_unix, uint16_t time_out);

bool isLeap(uint16_t y);							// якщо високосний рік то повертає true

uint8_t lastDayOfMonth(uint8_t m, uint16_t y);					// останній день місяця

String timeString(uint8_t h, uint8_t m, uint8_t s);				// отримати рядок часу формата ГГ:ХХ:СС

String dateString(uint8_t d, uint8_t m, uint8_t y);				// отримати рядок дати формата ДД.ММ.РРРР

String monthString(uint8_t m);
String dayWeekString(uint8_t wd);
String monthShortString(uint8_t m);
String dayWeekShortString(uint8_t wd);

String monthStringUA(uint8_t m);
String dayWeekStringUA(uint8_t wd);
String monthShortStringUA(uint8_t m);
String dayWeekShortStringUA(uint8_t wd);

String monthStringRU(uint8_t m);
String dayWeekStringRU(uint8_t wd);
String monthShortStringRU(uint8_t m);
String dayWeekShortStringRU(uint8_t wd);

void decodeUNIX(uint32_t unix, uint8_t& d, uint8_t& m, uint16_t& y);	// швидке декодування
void decodeUNIX(uint32_t unix, uint8_t& d, uint8_t& m, uint16_t& y, uint16_t& h, uint16_t& min, uint16_t& s);

void delay(uint32_t t, void (*func)());		// з функцією яка виконуеться підчас затримки (наприклад, TimeFunc.delay(1000, Loop);)
```

<a id="example"></a>
## Приклад testBlink.ino
```cpp
//EN
// example outputting time every second 
// and the LED flashes twice a second 
// you can flash several boards - they will flash synchronously
//UA
// приклад виводу часу кожну секунду
// а також два раза в секунду блимає світлодіодом
// можно прошити декілька плат - вони будуть мигати синхронно

#include <ESP8266WiFi.h>  // esp8266
//#include <WiFi.h>       // esp32

/*#include <Ethernet.h>
  #define NTPtimeEthernet   //Ethernet
  byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
  };
*/

#include <NTPtime.h>
NTPtime Time(3);

void setup() {
  Serial.begin(115200);
  
  WiFi.begin("WIFI_SSID", "WIFI_PASS");
  while (WiFi.status() != WL_CONNECTED) delay(100);
  Serial.println("Connected");

  /*Ethernet
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, IPAddress(192, 168, 0, 1));
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  */

Time.begin();
pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  Time.tick();

  if (Time.ms() == 0) {// секунда почалась / second started
    delay(1);
    digitalWrite(LED_BUILTIN, 0); 		//включити інвертований світлодіод esp8266 / turn on the esp8266(inverted) LED
    Serial.println(Time.timeString()); 	//виводимо / outputting
    Serial.println(Time.dateString());
    Serial.println();
  }
  if (Time.ms() == 500) {
    delay(1);
    digitalWrite(LED_BUILTIN, 1);
  }
}
````

<a id="versions"></a>
## Версії
- v1.0
- v1.1.0
- v1.1.1
    - Оптимізація
	- Добавлені нові функції
	    - `periodInFullDays();`
		- `isAM();`
        - `isPM();`
        - `hourFormat12();`
		- `decodeUNIX();` - швидке декодування
		- `delay()` з функцією яка виконуеться підчас затримки
- v1.1.2
	- Оптимізація
	- Виправлення помилок
	- Добавлені нові функції
		- `setHost(const String& host);`
		- `setPort(uint16_t port);`		
		- `setTimeout(uint16_t timeout);`
	- Інші важливі виправлення