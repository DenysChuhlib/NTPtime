#include <UNIXtime.h>
UNIXtime Timer;

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);

  Timer.startTime();
  uint8_t s;
  while (!Timer.everyS(s, 5)) delay(0); //чекаем / waiting
  if (Timer.everyS(s, 5)) digitalWrite(LED_BUILTIN, 1);       //включити світлодіод (на esp8266 виключити) / turn on the LED (turn off on esp8266)
  while (!Timer.everyS(s, 10)) delay(0); //чекаєм ще 5 секунд
  if (Timer.everyS(s, 10)) digitalWrite(LED_BUILTIN, 0);
}

void loop() {
  if (Timer.ms() == 0) {// секунда почалась / second started
    delay(1);
    digitalWrite(LED_BUILTIN, 0);         //включити світлодіод esp8266 / turn on the esp8266 LED
    Serial.println(Timer.timeString());   //виводимо / outputting
    Serial.println();
  }
  if (Timer.ms() == 500) {
    delay(1);
    digitalWrite(LED_BUILTIN, 1);
  }
}
