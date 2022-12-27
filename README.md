[![Foo](https://img.shields.io/badge/Library%20Manager-NTPtime%201.2.0-000282.svg?style=for-the-badge&logo=arduino)](https://www.ardu-badge.com/NTPtime)

[![Foo](https://img.shields.io/badge/README-English-fff700.svg?style=for-the-badge)](https://github-com.translate.goog/DenysChuhlib/NTPtime?_x_tr_sl=uk&_x_tr_tl=en)
[![Foo](https://img.shields.io/badge/README-Руский-fff700.svg?style=for-the-badge)](https://github-com.translate.goog/DenysChuhlib/NTPtime?_x_tr_sl=uk&_x_tr_tl=ru)

# NTPtime
Бібліотека часу з багатьма функціями:
- Отримання точного часу за допомогою NTP сервера і використання
- Використання UNIX часу і його декодування
- Можливість використання UNIX64 для більш ширших розрахунків (можна і від'ємний час обробляти)
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
- [Використання і особливості](#usage)
	- [Особливості](#features)
	- [DSTime](#DSTime)
	- [NTPtime](#NTPtime)
	- [UNIXtime і NTPtime](#UNIXtime&NTPtime)
	- [TimeFunc](#TimeFunc)
- [Версії](#versions)

<a id="install"></a>
## Установка
- Бібліотеку можна знайти за назвою `NTPtime` та встановити через менеджер бібліотек у:
	- Arduino IDE v2.0 `(рекомендовано)`
	- Arduino IDE
- [Завантажити бібліотеку](https://github.com/DenysChuhlib/NTPtime/archive/refs/heads/main.zip) .zip архівом для ручної установки:
	- Розпакувати та покласти в `Документи/Arduino/libraries/`
	- (Arduino IDE) автоматичне встановлення з .zip: `Скетч/Підключити бібліотеку/Додати .ZIP бібліотеку…` та вказати завантажений архів

<a id="usage"></a>
## Використання і особливості

### Режим UNIX64 (v1.2)
Для ввімкнення режиму необхідно, перед підлкюченням любої частини бібліотеки (`NTPTime.h`/`UNIXtime.h`/`TimeFunc.h`), написати `#define UNIX64`.
> PS: Якщо ядро вашої плати, а точніше бібліотека String.h не підтримує числа 64 біт, то буде помилка комбіляції. Можливо, в майб'тньому зроблю універсальніше =).

### Типи данних (v1.2)
- `unix_t` - тип для Unix часу. В звичайном режимі це `uint32_t` / якщо ввімкнений `UNIX64` то `int64_t`
- `year_t` - тип для року. В звичайном режимі це `uint16_t` / якщо ввімкнений `UNIX64` то `int64_t`
- `DSTime` - структира для зберігання данних які використовуються в обчисленні DST.

<a id="features"></a>
### Особливості
NTPTime
- `tick()`, `DST()`,`DSTauto()`, `updateOnTime()` працюють головному в циклі `loop()`.
- Чим більше потрібна точність часу тим меньше потрібен період в `tick(prd)`. Якщо простий годинник, достатньо одного updateOnTime(0,0,10) і обновляти в нічий час, коли найкращий Інтернет.

UNIXtime
- `DST()`,`DSTauto()` - працюють головному в циклі `loop()`.
- Мілісекунди зберігаються при зупинці відліку часу і відновлюються при запуску.

Для кращого розуміння, дивіться приклади `testBlink.ino`,`Watch.ino`,`Timer.ino`,`Timer_with_Time` та інші.

<a id="DSTime"></a>
### DSTime
```cpp
//Ініціалізація
DSTime(uint8_t month_start_, uint8_t week_start_, uint8_t dayWeek_start_, uint8_t h_start_,
	uint8_t month_end_, uint8_t week_end_, uint8_t dayWeek_end_, uint8_t h_end_, bool dst_or_wt_ = 1);

// для зміни всих значень
void setDST(uint8_t month_start_, uint8_t week_start_, uint8_t dayWeek_start_, uint8_t h_start_,
	uint8_t month_end_, uint8_t week_end_, uint8_t dayWeek_end_, uint8_t h_end_, bool dst_or_wt_ = 1);

//можна змінювати одне значення
//елементи з бітовими полями для ефективного зберігання в EEPROM. (займають лише 4 байта)
uint8_t month_start		: 4;
uint8_t week_start		: 3;
uint8_t dayWeek_start		: 3;
uint8_t h_start			: 5;
                      
uint8_t month_end		: 4;
uint8_t week_end		: 3;
uint8_t dayWeek_end		: 3;
uint8_t h_end			: 5;

uint8_t dst_or_wt		: 1 = 1;
```

<a id="NTPtime"></a>
### NTPtime
```cpp
//Ініціалізація
NTPtime Time; 						// параметри за замовчуванням (time zone 0, time zone minune)
NTPtime(time_zona, time_zonaM); 			// часовий пояс у годинах і хвилинах

void setHost(const String& host); 			// установити хост (за умовч. pool.ntp.org)
void setPort(uint16_t port);				// установити порт NTP сервера (за умовч. 123)
void setTimeout(uint16_t timeout);			// установити таймаут відповіді сервера (200 - 64000) (за умовч. 64000)

bool begin(uint16_t port); 				// запустити (вхідний порт за умовч. 123)
void end(); 						// зупинити

void asyncMode(bool _on_off); 				// асинхронний режим (за умовч. ввімкнений, true)

void updateNow(); 					// обновити зараз (ручний запуск оновлення)

bool updateOnTime(uint8_t h, uint8_t m, uint8_t s); 	// функція `loop()` цикла яка оновлює час в Г, Х, С (працює в парі з tick())

// функція `loop()` цикла яка оновлює час по періоду (1 хв <= prd <= 1 доба), після ручного запуска або в певний час, підчас оновлення повертає true
bool tick(uint16_t prd);

int16_t ping(); 					// отримати пінг сервера (при відсутньому з'єднанні повертає -1)

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

<a id="UNIXtime&NTPtime"></a>
### UNIXtime і NTPtime
В основі `NTPtime` лежить `UNIXtime` і він має такі функції:

```cpp
//Ініціалізація
UNIXtime Timer; 					// таймер (запустити Timer.startTime();)

//unix час, часовий пояс, хвилини часового поясу, міліскекунди
UNIXtime(unix_t unix, int8_t tz, int8_t tzM, uint16_t ms);

// рік, місяць, день, години, хвилини, секунди, часовий пояс у хвилинах, мілісекунди
UNIXtime(year_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM, uint16_t ms);

void setUnixGMT(unix_t unix, uint16_t ms); 		// встановити unix час відносно грінвіча

void setUnixFromDate(year_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM = 0, uint16_t ms = 0);

void addUnix(unix_t unix, uint16_t ms = 0);
void subtractUnix(unix_t unix, uint16_t ms = 0);

void setTimeZone(int8_t tz, int8_t tzM); 		// установити часовий пояс
int16_t getTimeZoneM();					// дізнатись часовий пояс в хвилинах

void setDST (int8_t dst_wt); 				// установити літній час 1 (за умовчуванням) та зимовий час -1 або стандартний час 0
void setSummerTime(); 					// установити літній час

void setWinterTime(); 					// установити зимовий час

void setStandardTime();					// установити стандартний час

int8_t getDST(); 					// поверне якщо літній час 1 та зимовий час -1 або стандартний час 0

void setDSTauto(DSTime * dst); 	// передача адреси структури DSTime для втановки автоматичного перерахунки і перевірки DST
void delDSTauto();		// очистити покажчик на елемент стрктури DSTime
bool DSTauto();			// функція ручного однократного запуска перерахунку DST за данними `всановленої` структури DSTime

bool DST (DSTime & dst);  // функція ручного однократного запуска перерахунку DST за данними `даної` структури DSTime

// функція ручного однократного запуска перерахунку DST за данними значеннями
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

unix_t DSTstartInGMT (uint8_t month_start, uint8_t week_start, uint8_t dayWeek_start, uint8_t h_start);
unix_t DSTendInGMT (uint8_t month_end, uint8_t week_end, uint8_t dayWeek_end, uint8_t h_end, bool dst_or_wt = 1);


// отримати статус системи
/*
    UNIX_OK 			0 - все ок
    UNIX_NOT_SYNCHRONIZED 	1 - не синхронізовано
    UNIX_NOT_STARTED 		2 - зупинено
*/	
bool status();

bool isRunning();

void startTime();					// почати відлік часу

void stopTime();					// зупунити відлік часу

uint32_t msFromUpdate(); 				// мілісекунд з останнього оновлення

unix_t unixGMT(); 					// unix час відносно грінвіча

unix_t unix(); 						// unix час відносно даного часового пояса

uint16_t ms(); 						// мілісекунди

uint8_t second(); 					// отримати секунди

uint8_t minute(); 					// отримати хвилини

uint8_t hour(); 					// отримати години

uint8_t day(); 						// отримати день місяця

uint8_t month(); 					// отримати місяць

year_t year(); 						// отримати рік

uint8_t dayWeek(); 					// отримати день тижня

bool isAM();
bool isPM();
uint8_t hourFormat12();

bool onTime (uint8_t h, uint8_t m, uint8_t s);		// назва функції говорить сама за себе

bool onDate (uint8_t d, uint8_t m, uint8_t y);		// назва функції говорить сама за себе

bool timeAfter (uint8_t h, uint8_t m, uint8_t s);	// назва функції говорить сама за себе

bool dateAfter (uint8_t d, uint8_t m, year_t y);	// назва функції говорить сама за себе

bool everyH(uint8_t time_last, uint8_t time_out);	// прості і маленькі таймери (time_last - останній збережений hour()) (time_out від 0 до 24)
bool everyM(uint8_t time_last, uint8_t time_out);	// (time_last - останній збережений minute()) (time_out від 0 до 60)
bool everyS(uint8_t time_last, uint8_t time_out);	// (time_last - останній збережений second()) (time_out від 0 до 60)
bool everyMs(uint16_t time_last, uint16_t time_out);	// (time_last - останній збережений ms()) (time_out від 0 до 1000)

unix_t periodInSec(unix_t last_unix);			// період в секундах (last_unix - останній збережений unix())
unix_t periodInDays(unix_t last_unix);
unix_t periodInFullDays(unix_t last_unix);
unix_t periodInMonths(unix_t last_unix);

bool timeOutMonth(unix_t last_unix, unix_t time_out);	// якщо період більше або дорівнює таймауту то повертає true
bool timeOutD(unix_t last_unix, unix_t time_out);
bool timeOutH(unix_t last_unix, unix_t time_out);
bool timeOutM(unix_t last_unix, unix_t time_out);
bool timeOutS(unix_t last_unix, unix_t time_out);

bool isLeap();						// якщо високосний рік то повертає true

uint8_t lastDayOfMonth();				// останній день місяця

void delay(uint32_t t, void (*func)());			// з функцією яка виконуеться підчас затримки

String timeString();					// отримати рядок часу формата ГГ:ХХ:СС

String dateString();					// отримати рядок дати формата ДД.ММ.РРРР

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
 TimeFunc.isLeap(year_t y); 
```
 і все.

```cpp

bool isAM(uint8_t h);
bool isPM(uint8_t h);
uint8_t hourFormat12(uint8_t h);

unix_t nextDays(unix_t unix, unix_t n); 				// додає до unix n днів
unix_t nextDay(unix_t unix); 						// повертає наступний день

unix_t prevDays(unix_t unix, unix_t n); 				// віднімає від unix на n днів
unix_t prevDay(unix_t unix);  						// повертає минулий день

bool everyH(uint8_t time_now, uint8_t time_last, uint8_t time_out);	// прості і маленькі таймери (time_last - останній збережений hour()) (time_out від 0 до 24)
bool everyM(uint8_t time_now, uint8_t time_last, uint8_t time_out);	// (time_last - останній збережена хвилина) (time_out від 0 до 60)
bool everyS(uint8_t time_now, uint8_t time_last, uint8_t time_out);	// (time_last - останній збережена секунда) (time_out від 0 до 60)
bool everyMs(uint16_t time_now, uint16_t time_last, uint16_t time_out);	// (time_last - останній збережена мілісекунда) (time_out від 0 до 1000)

unix_t periodInSec(unix_t unix_now, unix_t last_unix);			// період в секундах (last_unix - останній збережений unix())
unix_t periodInDays(unix_t unix_now, unix_t last_unix);
unix_t periodInFullDays(unix_t unix_now, unix_t last_unix);
unix_t periodInMonths(unix_t unix_now, unix_t last_unix);

bool timeOutMonth(unix_t unix_now, unix_t last_unix, unix_t time_out);	// якщо період більше або дорівнює таймауту то повертає true
bool timeOutD(unix_t unix_now, unix_t last_unix, unix_t time_out);
bool timeOutH(unix_t unix_now, unix_t last_unix, unix_t time_out);
bool timeOutM(unix_t unix_now, unix_t last_unix, unix_t time_out);
bool timeOutS(unix_t unix_now, unix_t last_unix, unix_t time_out);

bool isLeap(year_t y);							// якщо високосний рік то повертає true

uint8_t lastDayOfMonth(uint8_t m, year_t y);				// останній день місяця

unix_t unixGMTFromDate(year_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int16_t tzM);

void decodeUNIX(unix_t unix, uint8_t& d, uint8_t& m, year_t& y);	// швидке декодування
void decodeUNIX(unix_t unix, uint8_t& d, uint8_t& m, year_t& y, uint8_t& h, uint8_t& min, uint8_t& s);

uint8_t dayWeek(unix_t unix);

void delay(uint32_t t, void (*func)());	  // з функцією яка виконуеться підчас затримки (наприклад, TimeFunc.delay(1000, Loop);)

String timeString(uint8_t h, uint8_t m, uint8_t s);	// отримати рядок часу формата ГГ:ХХ:СС

String dateString(uint8_t d, uint8_t m, year_t y);	// отримати рядок дати формата ДД.ММ.РРРР

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

```

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
- v1.1.3
	- Добавлена підтримка UNIX 64bit
	- Оптимізований алгоритм DST
- v1.2.0
	- Оптимізація
	- Оновлений алгоритм DST
	- Добавлене автоматичне оновлення DST, при визові будь-яких функцій пов'язаних з часом (не налаштування)
	- Добавлена повна підтримка UNIX64
	- Добавлені типи даних
		- `unix_t`
		- `year_t`
		- `DSTime`
	- Добавлені нові функції
		- UNIXtime і NTPTime:
			- `addUnix(unix_t unix, uint16_t ms = 0);`
			- `subtractUnix(unix_t unix, uint16_t ms = 0);`
			- `setDSTauto(DSTime *dst);`
			- `delDSTauto();`		
			- `DSTauto();`
			- `DST(DSTime & dst);`		
			- `DSTstartInGMT(uint8_t month_start, uint8_t week_start, uint8_t dayWeek_start, uint8_t h_start);`			
			- `DSTendInGMT(uint8_t month_end, uint8_t week_end, uint8_t dayWeek_end, uint8_t h_end, bool dst_or_wt = 1);`
			- `isRunning();`
		- TimeFunc:
			- `nextDays(unix_t unix, unix_t n);`		
			- `nextDay(unix_t unix);`					
			- `prevDays(unix_t unix, unix_t n);`		
			- `prevDay(unix_t unix);`
