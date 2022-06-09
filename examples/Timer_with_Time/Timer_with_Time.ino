#include <ESP8266WiFi.h>  // esp8266
//#include <WiFi.h>       // esp32

#include <NTPtime.h>
NTPtime Time(3);

void setup() {
  Serial.begin(115200);
  WiFi.begin("WIFI_SSID", "WIFI_PASS");
  while (WiFi.status() != WL_CONNECTED) delay(100);
  Serial.println("Connected");

  Time.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  while (Time.status() != UNIX_OK) Time.tick();
  
  uint8_t s = Time.second(); 
  while (!Time.everyS(s, 5)) delay(0); 	//чекаем / waiting
  if (Time.everyS(s, 5)) {		// таймер який зважає тільки на секунди (з 12:20:02.200 до 12:20:07.000, тобто йому байдуже на мілісекунди або і нижчі значення)
    //timer that counts only for seconds (from 12: 20: 02.200 to 12: 20: 07.000, ie he does not care for milliseconds or lower values)
    
    digitalWrite(LED_BUILTIN, 1);       //включити світлодіод (на esp8266 виключити) 
    // turn on the LED (turn off on esp8266)
      
    s = Time.second();			//скидання таймаута 
    // reset the timeout
  }
  
  uint16_t ms = Time.ms();
  while (!Time.everyS(s, 5)) delay(0); //чекаем / waiting
  if (Time.everyS(s, 5) && Time.ms() == ms) { //можно додати точності добавивши мілісекунди / you can add accuracy by adding milliseconds
    digitalWrite(LED_BUILTIN, 0);
    s = Time.second();			
    ms = Time.ms();
  }
  
  ms = Time.ms();
  uint32_t lastUnix = Time.unix();
  while (!Time.timeOutS(lastUnix, 2)) delay(0); //чекаєм ще 2 секунд / wait another 2 seconds
  if (Time.timeOutS(lastUnix, 2) && Time.ms() == ms) {
    digitalWrite(LED_BUILTIN, 1);
    s = Time.second();
    ms = Time.ms();
  }
  
  /*
  uint8_t h = Time.hour(); 
  while (!Time.everyH(h, 3)) delay(0);
  if (Time.everyH(h, 3)) {		// таймер який зважає тільки на години (з 12:20:02.200 до 15:00:00.000, а потім в 18:00:00.000 і так кожну 3 години)
    //timer that counts only for hours (from 12: 20: 02.200 to 15: 00: 00.000, and then at 18: 00: 00.000 and so every 3 hours)
    
    digitalWrite(LED_BUILTIN,0);       	//включити світлодіод (на esp8266 виключити)
    // turn on the LED (turn off on esp8266)
    
    h = Time.hour();						//скидання таймаута
    // reset the timeout
  }
  */
  
  /*lastUnix = Time.unix();
	ms = Time.ms();
    while (!Time.timeOutD(lastUnix, 60)) delay(0); 			//чекаєм 60 днів / wait 60 days
    if (Time.timeOutD(lastUnix, 60) && Time.ms() == ms) { 		//60 днів з точністю до мілісекунди. У таймера на millis() максимум 49.7 днів.
    //60 days to the nearest millisecond. The millis() timer has a maximum of 49.7 days.
    digitalWrite(LED_BUILTIN, 1);
    lastUnix = Time.unix();
    ms = Time.ms();
    }
  */
}

void loop() {
  Time.tick();

  if (Time.ms() == 0) {// секунда почалась / second started
    delay(1);
    digitalWrite(LED_BUILTIN, 0);         //включити світлодіод esp8266 / turn on the LED (turn off on esp8266)
    Serial.println(Time.timeString());   //виводимо / outputting
    Serial.println();
  }
  if (Time.ms() == 500) {
    delay(1);
    digitalWrite(LED_BUILTIN, 1);
  }
}
