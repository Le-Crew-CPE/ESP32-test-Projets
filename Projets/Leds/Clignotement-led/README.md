# ESP32-test-Projets
Projet de test pour découvrir les options de l'ESP32 (version ESP32 S3)




## Commandes


Passer en mode bootloader : 

- Maintenir BOOT 
- Cliquer sur RST
- Relacher BOOT

Puis pour téléverser votre code éxécuter : `pio run -t upload` 
```
pio run                      # compile
pio run -t upload            # compile + flashe
pio run -t upload -t monitor # compile, flashe, ouvre le moniteur série
pio device list              # liste les ports détectés
```
