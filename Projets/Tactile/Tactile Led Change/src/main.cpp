#include <Arduino.h>


void setup() {
  Serial.begin(115200);
  delay(1000); // Delai pour lancer le moniteur série
  Serial.println("ESP32 Touch Demo");
}

void loop() {
  Serial.println(touchRead(4));
  delay(500);
}