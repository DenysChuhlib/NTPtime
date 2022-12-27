#include <ESP8266WiFi.h>  // esp8266
//#include <WiFi.h>       // esp32

#include <NTPtime.h>
NTPtime Time(2); //UA in +2 time zone
DSTtime dst(2, 0, 7, 3, 10, 0, 7, 4); //https://en.wikipedia.org/wiki/Eastern_European_Summer_Time

void setup() {
  Serial.begin(115200);
  
  WiFi.begin("WIFI_SSID", "WIFI_PASS");
  while (WiFi.status() != WL_CONNECTED) delay(10);
  Serial.println("Connected");

  Time.setDSTauto(&dst);
  Time.begin();
}

void loop() {
  //Time.DST(2, 0, 7, 3, 10, 0, 7, 4); //manually calculate DST
  Time.updateOnTime(0,0,10);
  Time.tick();
  
  if(Time.everyMs(0,1000)) {
    Serial.println(Time.timeString());
	UNIXtime moment1 = Time; //save moment time and other settings
	
	//тепер moment1 має час, таймзону і покажчик на структуру dst (з данним для автоматичного розрахунку)

	//now moment1 has a time, a timezone and a pointer to the dst structure (with data for automatic calculation)
	Serial.println(moment1.timeString());
	Serial.println(String(F("Time Zone in minute")) + moment1.getTimeZoneM());
	Serial.println(String(F("DST is")) + moment1.getDST());
	
	//автоматичне обрахування DST відбуваэться якщо визиваються будьякі функції для отримання данних з часу (функції з найстройками не рахуються)
	//але можна обрахувати і вручну
	
	//automatic calculation of DST occurs if any functions are called to obtain time data (functions with settings are not counted)
	//but you can also calculate manually
	moment1.DSTauto();
	
	//напочатку в Time об'єкту класу NTPtime завжи йде час
	//а в UNIXtime ні і тому час там залишається на місці, але його можна запустити функцією startTime(); і він піде від збереженого моменту
	
	//initially, time always passes in the Time object of the NTPtime class
	//and in UNIXtime it is not, and therefore the time there remains in place, but it can be started with the function startTime(); and it will go from the saved moment
	moment1.startTime(); 
	delay(1000);
	Serial.println(String(F("Time:"))Time.timeString());
	Serial.println(String(F("moment1:"))moment1.timeString());
  }
}