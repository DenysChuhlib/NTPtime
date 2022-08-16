# NTPtime
Библиотека времени со многими функциями:
- получение точного времени с помощью NTP
- Альтернатива стандартной библиотеке `Time.h`
- Возможность делать таймер в равное время, например ровно в 12:00 и так ежечасно, занимающий только один байт
- Можно делать целый календарь

### Совместимость всей библиотеки
Любые ардуино совместимые платы

### Совместимость синхронизации NTP
esp8266, esp32, Ethernet

## Содержание
- [Установка](#install)
- [Инициализация](#init)
- [Использование](#usage)
    - [NTPtime](#NTPtime)
    - [UNIXtime і NTPtime](#UNIXtime&NTPtime)
    - [TimeFunc](#TimeFunc)
- [Пример](#example)
- [Версии](#versions)

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию `NTPtime` и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
- [Загрузить библиотеку](https://github.com/DenysChuhlib/NTPtime/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в `Документы/Arduino/libraries/`
    - (Arduino IDE) автоматическая установка с .zip: `Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…` и указать загруженный архив

<a id="init"></a>
## Инициализация
```cpp
NTPtime Time; 							// параметры по умолчанию (time zone 0, time zone minune)
NTPtime(time_zona, time_zonaM); 				// часовой пояс в часах и минутах

UNIXtime Timer; 						// Таймер (запустить Timer.startTime();)
UNIXtime(uint32_t unix, int8_t tz, int8_t tzM, uint16_t ms) //unix время, часовой пояс, минуты часового пояса, миллискекунди
UNIXtime(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM, uint16_t ms) // год, месяц, день, часы, минуты, секунды, секунды, часовой пояс в минутах,
````
<a id="usage"></a>
## Использование
<a id="NTPtime"></a>
### NTPtime
```cpp

void setHost(const char* host); 				// установить хост (по умвч. pool.ntp.org)

bool begin(uint16_t port = _NTPtime_NTP_PORT); 			// запустить обновление
void end(); 							// остановить обновление

void asyncMode(bool _on_off); 					// асинхронный режим (по умолч. включен, true)

void updateNow(); 						// обновить сейчас (ручной запуск обновления)

bool updateOnTime(uint8_t h, uint8_t m, uint8_t s); 		// функция `loop()` цикла обновляющая время в Г, Х, С (работает в паре с tick())

bool tick(uint16_t prd); 					// функция `loop()` цикла обновляющая время по периоду (prd), после ручного запуска или в определенное время, во время обновления возвращает true

int16_t ping(); 						// получить пинг сервера

// получить статус обновления
/*
    NTP_OK 				0 – все ок
    NTP_NOT_STARTED 			1 - не запущен UDP
    NTP_NOT_CONNECTED_WIFI 		2 – не подключен WiFi
    NTP_NOT_CONNECTED_TO_SERVER 	3 – ошибка подключения к серверу
    NTP_NOT_SENT_PACKET 		4 – ошибка отправки пакета
    NTP_WAITING_REPLY 			5 – идет ожидание ответа
    NTP_TIMEOUT 			6 – таймаут ответы сервера
    NTP_REPLY_ERROR 			7 – полученный некорректный ответ сервера
*/
uint8_t NTPstatus();
```
<a id="UNIXtime&NTPtime"></a>
### UNIXtime и NTPtime

```cpp
void setUnixGMT(uint32_t unix, uint16_t ms);                    // установить unix время относительно гринвича

void setUnixFromDate(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM = 0, uint16_t ms = 0); // название функции говорит само за себя

void setTimeZone(int8_t tz, int8_t tzM);                        // установить часовой пояс
int16_t getTimeZoneM();                                         // узнать часовой пояс в минутах

void setDST(int8_t dst_wt);                                     // установить летнее время 1 (по умолчанию) и зимнее время -1 или стандартное время 0
void setSummerTime();                                           // установить летнее время

void setWinterTime();                                           // установить зимнее время

void setStandardTime();                                         // установить стандартное время

int8_t getDST();                                                // вернет, если летнее время 1 и зимнее время -1 или стандартное время 0

void DST(uint8_t month_start, uint8_t week_start, uint8_t dayWeek_start, uint8_t h_start,
uint8_t month_end, uint8_t week_end, uint8_t dayWeek_end, uint8_t h_end, bool dst_or_wt);
//month_start – месяц начала летнего/зимнего времени
//week_start - неделя начала летнего/зимнего времени (если 0 то последняя неделя)
//dayWeek_start – день недели начала летнего/зимнего времени
//h_start – час начала летнего/зимнего времени

//month_end – месяц конца летнего/зимнего времени
//week_end - неделя конца летнего/зимнего времени (если 0 то последняя неделя)
//dayWeek_end - день недели конца летнего/зимнего времени
//h_end – час конца летнего/зимнего времени

//dst_or_wt по умолчанию 1 – летнее время, если 0 то зимнее
//DST(2, 0, 7, 3, 10, 0, 7, 4); //https://en.wikipedia.org/wiki/Eastern_European_Summer_Time

// получить статус системы
/*
    UNIX_OK                 0 – все ок
    UNIX_NOT_SYNCHRONIZED   1 – не синхронизировано
    UNIX_NOT_STARTED        2 - остановлено
*/
bool status();

void startTime();                                               // начать отсчет времени

void stopTime();                                                // сбросить отсчет времени

uint32_t msFromUpdate();                                        // миллисекунд последнего обновления

uint32_t unixGMT();                                             // unix время в отношении гринвича

uint32_t unix();                                                // unix время в отношении данного часового пояса

uint16_t ms();                                                  // миллисекунды

uint8_t second();                                               // получить секунды

uint8_t minute();                                               // получить минуты

uint8_t hour();                                                 // получить часы

uint8_t day();                                                  // получить день месяца

uint8_t month();                                                // получить месяц

uint16_t year();                                                // получить год

uint8_t dayWeek();                                              // получить день недели

bool isAM();
bool isPM();
uint8_t hourFormat12();

bool onTime (uint8_t h, uint8_t m, uint8_t s);                  // название функции говорит само за себя

bool onDate (uint8_t d, uint8_t m, uint8_t y);                  // название функции говорит само за себя

bool timeAfter (uint8_t h, uint8_t m, uint8_t s);               // название функции говорит само за себя

bool dateAfter (uint8_t d, uint8_t m, uint8_t y);               // название функции говорит само за себя

bool everyH(uint8_t time_last, uint8_t time_out);               // простые и маленькие таймеры (time_last - последний сохраненный hour()) (time_out от 0 до 24)
bool everyM(uint8_t time_last, uint8_t time_out);               // (time_last - последний сохранен minute()) (time_out от 0 до 60)
bool everyS(uint8_t time_last, uint8_t time_out);               // (time_last - последний сохраненный second()) (time_out от 0 до 60)
bool everyMs(uint16_t time_last, uint16_t time_out);            // (time_last - последний сохраненный ms()) (time_out от 0 до 1000)

uint32_t periodInSec(uint32_t last_unix);                       // период в секундах (last_unix – последний сохраненный unix())
uint16_t periodInDays(uint32_t last_unix);
uint16_t periodInFullDays(uint32_t last_unix);
uint16_t periodInMonths(uint32_t last_unix);

bool timeOutMonth(uint32_t unix_now, uint32_t last_unix, uint16_t time_out);    // если период больше или равен таймауту, то возвращает true
bool timeOutD(uint32_t last_unix, uint16_t time_out);
bool timeOutH(uint32_t last_unix, uint16_t time_out);
bool timeOutM(uint32_t last_unix, uint16_t time_out);
bool timeOutS(uint32_t last_unix, uint16_t time_out);

bool isLeap();                                                  // если високосный год возвращает true

uint8_t lastDayOfMonth();                                       // последний день месяца

void delay(uint32_t t, void (*func)());							// с функцией выполняемой при задержке

String timeString();                                            // получить строчку времени формата ГГ:ХХ:СС

String dateString();                                            // получить строку даты формата ДД.ММ.ГГГГ

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
<a id="TimeFunc"></a>
### TimeFunc
Просто

```cpp
 TimeFunc.isLeap(uint16_t y);
``` 
и все.

```cpp

bool isAM(uint8_t h);
bool isPM(uint8_t h);
uint8_t hourFormat12(uint8_t h);

bool everyH(uint8_t time_now, uint8_t time_last, uint8_t time_out);     // простые и маленькие таймеры (time_last - последний сохраненный hour()) (time_out от 0 до 24)
bool everyM(uint8_t time_now, uint8_t time_last, uint8_t time_out);     // (time_last – последний сохраненная минута) (time_out от 0 до 60)
bool everyS(uint8_t time_now, uint8_t time_last, uint8_t time_out);     // (time_last – последний сохраненная секунда) (time_out от 0 до 60)
bool everyMs(uint16_t time_now, uint16_t time_last, uint16_t time_out); // (time_last – последний сохраненная миллисекунда) (time_out от 0 до 1000)

uint32_t periodInSec(uint32_t unix_now, uint32_t last_unix);    // период в секундах (last_unix – последний сохраненный unix())
uint16_t periodInDays(uint32_t unix_now, uint32_t last_unix);
uint16_t periodInFullDays(uint32_t unix_now, uint32_t last_unix);
uint16_t periodInMonths(uint32_t unix_now, uint32_t last_unix);

bool timeOutMonth(uint32_t unix_now, uint32_t last_unix, uint16_t time_out);    // если период больше или равен таймауту, то возвращает true
bool timeOutD(uint32_t unix_now, uint32_t last_unix, uint16_t time_out);
bool timeOutH(uint32_t unix_now, uint32_t last_unix, uint16_t time_out);
bool timeOutM(uint32_t unix_now, uint32_t last_unix, uint16_t time_out);
bool timeOutS(uint32_t unix_now, uint32_t last_unix, uint16_t time_out);

bool isLeap(uint16_t y);                                        // если високосный год возвращает true

uint8_t lastDayOfMonth(uint8_t m, uint16_t y);                  // последний день месяца

String timeString(uint8_t h, uint8_t m, uint8_t s);             // получить строчку времени формата ЧЧ:ММ:СС

String dateString(uint8_t d, uint8_t m, uint8_t y);             // получить строку даты формата ДД.ММ.ГГГГ

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

void decodeUNIX(uint32_t unix, uint8_t& d, uint8_t& m, uint16_t& y);	// быстрое декодирование
void decodeUNIX(uint32_t unix, uint8_t& d, uint8_t& m, uint16_t& y, uint16_t& h, uint16_t& min, uint16_t& s);

void delay(uint32_t t, void (*func)());		// с функцией выполняемой при задержке (например, TimeFunc.delay(1000, Loop);)

```

### Особенности
- `tick()`, `DST()`, `updateOnTime()` работают главном в цикле `loop()`

<a id="example"></a>
## Пример testBlink.ino
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
  #define NTPtimeEthernetUdp   //Ethernet
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
## Версии
- v1.0
- v1.1.0
– v1.1.1
    - оптимизация
    - добавлены новые функции
        - `periodInFullDays();`
        - `isAM();`
        - `isPM();`
        - `hourFormat12();`
        - `decodeUNIX();` - быстрое декодирование
        - `delay()` с функцией выполняемой при задержке
