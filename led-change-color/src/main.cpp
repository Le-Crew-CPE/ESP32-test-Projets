#include <Arduino.h>

const int BOUTON = 0; // GPIO 0 = bouton BOOT
const int SEUIL_LONG = 600; // ms au-delà desquels c'est un appui long

int couleur = 0;
bool etatPrecedent = HIGH;
unsigned long tAppui = 0, tDernierChangement = 0;

void afficherCouleur(int c) {
  switch (c) {
    case 0: rgbLedWrite(RGB_BUILTIN, 30, 0, 0); break;  // rouge
    case 1: rgbLedWrite(RGB_BUILTIN, 0, 30, 0); break;  // vert
    case 2: rgbLedWrite(RGB_BUILTIN, 0, 0, 30); break;  // bleu
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BOUTON, INPUT_PULLUP);
  afficherCouleur(couleur);
  Serial.println("Pret. Appui court = couleur, appui long = extinction.");
}

void loop() {
  bool etat = digitalRead(BOUTON);

  if (etat != etatPrecedent && millis() - tDernierChangement > 30) {
    tDernierChangement = millis();
    etatPrecedent = etat;

    if (etat == LOW) {
      tAppui = millis();                        // debut de l'appui
    } else {
      unsigned long duree = millis() - tAppui;  // fin de l'appui
      if (duree < SEUIL_LONG) {
        couleur = (couleur + 1) % 3;
        Serial.printf("Court -> couleur %d\n", couleur);
        afficherCouleur(couleur);
      } else {
        Serial.printf("Long (%lu ms) -> extinction\n", duree);
        rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
      }
    }
  }
}