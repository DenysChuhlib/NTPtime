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