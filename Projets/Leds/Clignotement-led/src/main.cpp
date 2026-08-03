#include <Arduino.h>


int couleur = 0;
const int DELAY = 200;

void afficherCouleur(int c) {

  if (c == 0){
    rgbLedWrite(RGB_BUILTIN, 30, 0, 0); //Rouge
    
  } else if (c == 1) {
    rgbLedWrite(RGB_BUILTIN, 0, 30, 0); //Vert
    
  }else if (c == 2){
    rgbLedWrite(RGB_BUILTIN, 0, 0, 30); //Bleu
    
  }else{
    rgbLedWrite(LED_BUILTIN, 0, 0, 0); // Rien (down)
    
  }

}
void setup() {
  // Rien ? 
  Serial.begin(115200);

  afficherCouleur(couleur);
  Serial.println("Pret. Appui court = couleur, appui long = extinction.");
  
}

void loop() {
  couleur = 0;
  afficherCouleur(couleur);
  delay(DELAY);

  couleur = 1;
  afficherCouleur(couleur);
  delay(DELAY);

  couleur = 2;
  afficherCouleur(couleur);
  delay(DELAY);
  
  // Éteindre la LED
  couleur = 3;
  afficherCouleur(couleur);
  delay(DELAY);

}