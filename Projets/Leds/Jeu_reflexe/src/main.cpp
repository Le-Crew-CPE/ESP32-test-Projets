#include <Arduino.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int BUTTON = 0;
int etatPrecedent = HIGH;
unsigned long tempsDepart = 0;
int couleur = 0;

void CaptureTime() {
  if (tempsDepart > 0) {
    unsigned long duree = millis() - tempsDepart;
    Serial.printf("Ça a pris %lu ms\n", duree);
    tempsDepart = 0; // Réinitialise le temps
  }
}


int Aleatoire_func(){
  srand(time(NULL)); // Initialise le tmp

  int time_space = rand() %11; // Temp aléatoire entre 0 et 10 secondes
  return time_space;
}

void ChangeColors(int couleur){
  if (couleur == 0){
    rgbLedWrite(RGB_BUILTIN, 30, 0, 0); // Rouge
  }else if (couleur == 1){
    rgbLedWrite(RGB_BUILTIN, 0, 30, 0); // Vert
  }else if (couleur == 2){
    rgbLedWrite(RGB_BUILTIN, 0, 0, 30); // Bleu
  }else if (couleur == 3){
    rgbLedWrite(RGB_BUILTIN, 0, 255, 250); // Cyan
  }else if (couleur == 4){
    rgbLedWrite(RGB_BUILTIN, 242, 0, 255); // Magenta
  }else if (couleur == 5){
    rgbLedWrite(RGB_BUILTIN, 0, 255, 42); // Lime
  }else if (couleur == 6){
    rgbLedWrite(RGB_BUILTIN, 255, 123, 0); // Orange
  }else {
    rgbLedWrite(RGB_BUILTIN, 0, 0, 0); // Down
  }
  

}


void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.begin(115200);

}

void loop() {
  //Aleatoire_func();
  
  
  int etatActuel = digitalRead(BUTTON);

  if (etatActuel == LOW && etatPrecedent == HIGH) {
    Serial.println("Bouton appuyé");
    couleur++;
    if (couleur < 7){
      ChangeColors(couleur);
    }else {
      couleur = 0;
      ChangeColors(couleur);
    }
    


    // Ajouter ici un debounce logiciel si nécessaire
  }
  etatPrecedent = etatActuel;


  
}

