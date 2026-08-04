#include <Arduino.h>

const int LUMINOSITE = 20;   // 0-255 — la LED intégrée est éblouissante à fond
const int PAS = 20;          // ms entre deux teintes

// Position 0-255 sur la roue des couleurs -> R, G, B
void roueCouleur(uint8_t pos, uint8_t &r, uint8_t &g, uint8_t &b) {
  if (pos < 85) {                          // rouge -> vert
    r = 255 - pos * 3;  g = pos * 3;        b = 0;
  } else if (pos < 170) {                  // vert -> bleu
    pos -= 85;
    r = 0;
    g = 255 - pos * 3;
    b = pos * 3;
  } else {                                 // bleu -> rouge
    pos -= 170;
    r = pos * 3;        g = 0;              b = 255 - pos * 3;
  }
}

void afficherCouleur(uint8_t r, uint8_t g, uint8_t b) {
  rgbLedWrite(RGB_BUILTIN, r * LUMINOSITE / 255, g * LUMINOSITE / 255, b * LUMINOSITE / 255);
  
}

void setup() {
  Serial.begin(115200);
  Serial.println("Cycle de couleurs");
  
}

void loop() {
  for (int pos = 0; pos < 256; pos++) {
    uint8_t r, g, b;
    roueCouleur(pos, r, g, b);
    afficherCouleur(r, g, b);
    delay(PAS);
  }
}