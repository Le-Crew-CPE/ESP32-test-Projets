#include <Arduino.h>

const int BUTTON = 0;          // Bouton BOOT sur la plupart des cartes ESP32-S3
const uint8_t LUM = 20;        // Luminosité globale (0-255), la LED intégrée est très vive

// ---------- Utilitaires ----------

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  rgbLedWrite(RGB_BUILTIN, r, g, b);
}

bool boutonAppuye() {
  return digitalRead(BUTTON) == LOW;   // INPUT_PULLUP : LOW = appuyé
}

// ---------- Programme ----------

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON, INPUT_PULLUP);

  // Graine matérielle
  randomSeed(esp_random());

  setColor(0, 0, 0);
  Serial.println("Jeu de reflexes - appuie quand la LED passe au vert");
}

void loop() {
  // --- Phase 1 : compte a rebours ---
  setColor(LUM, 0, 0);                 // Rouge
  delay(1000);
  setColor(LUM, LUM / 2, 0);           // Orange
  delay(1000);
  setColor(LUM / 2, LUM / 2, LUM / 2); // Blanc : "prepare-toi"

  // --- Phase 2 : attente aleatoire (2 a 6 secondes) ---
  unsigned long attente = random(2000, 6001);   // en MILLISECONDES
  unsigned long debutAttente = millis();
  bool fauxDepart = false;

  while (millis() - debutAttente < attente) {
    if (boutonAppuye()) {      // appui trop tot
      fauxDepart = true;
      break;
    }
    delay(1);
  }

  if (fauxDepart) {
    Serial.println("Faux depart ! Attends le vert.");
    setColor(LUM, 0, 0);
    delay(1500);
    setColor(0, 0, 0);
    delay(1000);
    return; // on recommence une manche
  }

  // --- Phase 3 : GO ---
  setColor(0, LUM, 0); // Vert
  unsigned long depart = micros();

  while (!boutonAppuye()) {
    // attente active : on ne fait rien d'autre, precision maximale
  }

  unsigned long fin = micros();
  float ms = (fin - depart) / 1000.0f;

  Serial.printf("Temps de reaction : %.2f ms\n", ms);

  // --- Phase 4 : fin de manche ---
  setColor(0, 0, LUM);         // Bleu = manche terminee
  delay(2000);
  setColor(0, 0, 0);
  delay(500);
}
