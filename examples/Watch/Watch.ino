#include <ESP8266WiFi.h>  // esp8266
//#include <WiFi.h>       // esp32

#include <NTPtime.h>
NTPtime Time(2); //UA in +2 time zone

void setup() {
  Serial.begin(115200);
  
  WiFi.begin("WIFI_SSID", "WIFI_PASS");
  while (WiFi.status() != WL_CONNECTED) delay(10);
  Serial.println("Connected");

  Time.begin();
}

void loop() {
  Time.DST(2, 0, 7, 3, 10, 0, 7, 4); //https://en.wikipedia.org/wiki/Eastern_European_Summer_Time
  Time.updateOnTime(0,0,10);
  Time.tick();
  if(Time.everyMs(0,1000)) Serial.println(Time.timeString());
}