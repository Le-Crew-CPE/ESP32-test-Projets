#include "Arduino.h"

void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Demarrage OK");
  }
  
void loop() {
    Serial.printf("Temp puce : %.2f °C\n", temperatureRead());
    delay(1000);
  }