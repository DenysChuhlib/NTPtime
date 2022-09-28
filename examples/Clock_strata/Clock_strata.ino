
#include <ESP8266WiFi.h>  // esp8266
//#include <WiFi.h>       // esp32

//#include <Ethernet.h>
//#define NTPtimeEthernet   //Ethernet

uint8_t Clock_strata; //https://en.wikipedia.org/wiki/Network_Time_Protocol#Clock_strata
#define NTPtimeClockStrata_val Clock_strata

#include <NTPtime.h>
NTPtime Time(3);

void setup() {
  Serial.begin(9600);
  WiFi.begin("WIFI_SSID", "WIFI_PASS");
  while (WiFi.status() != WL_CONNECTED) delay(100);
  Serial.println("Connected");

  Time.begin();
}

void loop() {
  if (Time.tick() && Time.status() == 0) { 	//якщо час синхронізувався / if the time was synchronized
    Serial.println(Clock_strata);			//виводимо шар часового серверу / output the layer of the time server
  }

  delay(500);
}
