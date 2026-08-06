# Feuille de route robotique — 55 projets sur ESP32-S3

De la première LED au PAMI homologué pour la Coupe de France de robotique.

---

## Comment lire ce document

Chaque projet suit le même format :

- **Le projet** — ce que tu construis et pourquoi ça compte
- **Ce que tu apprends** — les technologies et concepts que tu ramènes du projet
- **Matériel** — ce qu'il te faut en plus de l'ESP32
- **Piège classique** — l'erreur que tout le monde fait, quand il y en a une

L'ordre compte. Les projets 34 et 35 sont les deux verrous techniques du PAMI : tant qu'ils ne sont pas maîtrisés, le reste de la catégorie 5 est hors de portée. Tout ce qui vient avant existe pour te préparer à ces deux-là.

Les projets 51 à 55 sont tes projets personnels. Ils ne sont pas plus faciles, ils dépendent simplement de compétences acquises ailleurs dans la liste — les prérequis sont indiqués.

---

## Sommaire

| Catégorie | Projets | Thème |
|---|---|---|
| 1 | 01 – 10 | Les bases de la robotique |
| 2 | 11 – 20 | Wifi et communications |
| 3 | 21 – 30 | Capteurs et affichage |
| 4 | 31 – 40 | Motorisation et déplacement |
| 5 | 41 – 50 | Le tout ensemble : ton PAMI |
| 6 | 51 – 55 | Tes projets personnels |

---

# Catégorie 1 — Les bases de la robotique

Ces dix projets ne produisent rien de spectaculaire. Ils installent les réflexes sans lesquels tout le reste devient impossible à déboguer. Ne les saute pas : chacun résout un problème que tu rencontreras à nouveau, en pire, sur le robot.

---

### 01 · Clignotement sans `delay()`

**Le projet.** Tu reprends ton blink, mais tu supprimes tous les `delay()`. À la place, tu compares l'heure courante à l'heure du dernier changement d'état. Ça paraît anodin, c'est en réalité le passage d'un programme qui fait une chose à un programme qui peut en faire plusieurs. Un robot doit lire ses capteurs pendant que ses moteurs tournent et que sa radio écoute — un seul `delay()` bloquant et tout s'arrête.

**Ce que tu apprends.** Gestion du temps non bloquante avec `millis()`, notion de machine à états, structure d'une boucle principale, multitâche coopératif, premier réflexe d'architecture logicielle.

**Matériel.** LED 5 mm, résistance 220 Ω.

**Piège classique.** Le compteur `millis()` déborde après 49 jours. La soustraction en arithmétique non signée gère ça toute seule — à condition d'écrire `if (millis() - dernier >= periode)` et jamais `if (millis() >= dernier + periode)`.

---

### 02 · Bouton, anti-rebond et appui long

**Le projet.** Un contact mécanique ne se ferme jamais proprement : il rebondit pendant quelques millisecondes et génère des dizaines de fronts. Tu écris le filtre logiciel qui élimine ces parasites, puis tu enrichis : appui court, appui long, double-clic. Tu réutiliseras ce module toute ta vie de roboticien.

**Ce que tu apprends.** Résistance de tirage (pull-up) interne, lecture d'entrée numérique, filtrage temporel, détection de front montant et descendant, hystérésis, conception d'un module réutilisable.

**Matériel.** Bouton tactile.

**Piège classique.** Sans pull-up activée, l'entrée flotte et capte l'électricité ambiante. Ton programme voit des appuis fantômes et tu accuses le bouton.

---

### 03 · Faire respirer une LED en PWM

**Le projet.** Tu découvres le PWM matériel de l'ESP32 (le périphérique LEDC), qui génère un signal carré à rapport cyclique variable sans consommer une seule instruction du processeur. Puis tu constates qu'une variation linéaire du rapport cyclique ne donne pas une variation linéaire de luminosité perçue, et tu appliques une correction gamma.

**Ce que tu apprends.** Modulation de largeur d'impulsion, fréquence et résolution, différence entre périphérique matériel et génération logicielle, perception non linéaire de l'œil, courbe gamma, arithmétique entière.

**Matériel.** LED, résistance.

---

### 04 · Lire un potentiomètre

**Le projet.** Convertir une tension continue en nombre. Tu vas vite découvrir que le convertisseur analogique-numérique de l'ESP32 est bruité et non linéaire, et que la valeur brute saute de plusieurs unités entre deux lectures identiques. Tu implémentes une moyenne glissante, puis un filtre passe-bas du premier ordre, et tu compares.

**Ce que tu apprends.** Conversion analogique-numérique, quantification, bruit de mesure, moyenne glissante, filtre exponentiel, calibration, atténuation d'entrée, compromis entre réactivité et stabilité.

**Matériel.** Potentiomètre 10 kΩ.

**Piège classique.** Sur ESP32, les entrées ADC2 deviennent inutilisables dès que le wifi est actif. Reste sur GPIO 1 à 10.

---

### 05 · Mini-shell sur le port série

**Le projet.** Tu écris un interpréteur de commandes textuelles : `led on`, `pwm 128`, `status`. Lecture caractère par caractère, découpage en mots, table de commandes associée à des pointeurs de fonction. C'est le premier code que tu écris pour toi-même plutôt que pour la machine, et c'est l'outil qui te fera gagner le plus de temps sur les 50 projets suivants.

**Ce que tu apprends.** Communication série UART, conception d'un protocole texte, manipulation de chaînes en C, pointeurs de fonction, table de dispatch, conception d'API, notion d'outillage de développement.

**Matériel.** Rien de plus.

---

### 06 · LED RGB et espace colorimétrique

**Le projet.** Trois canaux PWM pour trois couleurs primaires. Tu écris toi-même la conversion HSV vers RGB, qui permet de balayer la roue chromatique en faisant varier un seul paramètre au lieu de trois. C'est ta première fonction mathématique pure et réutilisable — tu la reprendras telle quelle pour tes LED de chambre.

**Ce que tu apprends.** Synthèse additive des couleurs, espaces colorimétriques, virgule flottante contre arithmétique entière, écriture d'une fonction sans effet de bord, réutilisabilité du code.

**Matériel.** LED RGB, 3 résistances.

---

### 07 · Buzzer et retour sonore

**Le projet.** Générer des fréquences audibles, jouer une mélodie stockée dans un tableau constant, puis concevoir un vocabulaire sonore : démarrage, erreur, batterie faible, capteur muet. Sur un robot en compétition, ce vocabulaire remplace l'écran et le câble USB — tu sais ce que fait la machine rien qu'en l'écoutant.

**Ce que tu apprends.** Génération de fréquence, stockage de données constantes en flash, séquencement temporel, conception d'une interface homme-machine sans écran, ergonomie de diagnostic.

**Matériel.** Buzzer passif.

---

### 08 · Feu tricolore avec bouton piéton

**Le projet.** Ton premier automate complet : plusieurs états, des transitions déclenchées soit par le temps soit par un événement extérieur. Tu le sépares en un fichier `.c` et un fichier `.h` avec une interface publique minimale. C'est le modèle exact de la machine à états de match qui pilotera ton PAMI.

**Ce que tu apprends.** Machine à états finis, séparation interface/implémentation, gardes d'inclusion, compilation séparée, conception modulaire, encapsulation.

**Matériel.** 6 LED, bouton.

**Piège classique.** Gérer l'état avec des booléens éparpillés (`estRouge`, `pietonAttend`, `enTransition`) rend le code impossible à déboguer dès la troisième condition. Un `enum`, une variable d'état, un `switch`.

---

### 09 · Deux tâches FreeRTOS sur deux cœurs

**Le projet.** L'ESP32-S3 a deux cœurs qui ne demandent qu'à travailler. Tu crées deux tâches indépendantes — l'une clignote, l'autre écoute le port série — et tu les fais communiquer par une file de messages plutôt que par une variable partagée. Tu découvres l'ordonnanceur, les priorités, et les bugs de concurrence.

**Ce que tu apprends.** Système d'exploitation temps réel, ordonnanceur préemptif, tâches et priorités, files de messages, sémaphores et mutex, sections critiques, épinglage sur un cœur, notion de concurrence.

**Matériel.** Rien de plus.

**Piège classique.** Partager une variable entre deux tâches sans protection produit un bug qui n'apparaît qu'une fois sur mille et jamais quand tu l'observes. C'est la classe de bugs la plus coûteuse en robotique.

---

### 10 · Deep sleep et calcul d'autonomie

**Le projet.** Tu endors la puce, tu la réveilles sur un timer puis sur un bouton, et tu mesures la consommation dans chaque mode. Puis tu calcules combien de temps ta batterie tiendrait dans chaque configuration. C'est le moment où l'énergie cesse d'être abstraite.

**Ce que tu apprends.** Modes de gestion d'énergie, sources de réveil, mémoire RTC persistante entre deux sommeils, calcul d'autonomie en mAh, courant moyen contre courant crête, mesure de courant.

**Matériel.** INA219 ou multimètre.

---

# Catégorie 2 — Wifi et communications

Ton ESP32 n'est pas qu'un microcontrôleur : c'est une radio. Cette catégorie te fait passer de l'objet isolé au système distribué, et c'est elle qui relie ta robotique à ton homelab.

---

### 11 · Serveur web embarqué

**Le projet.** L'ESP32 se connecte à ton réseau, sert une page HTML, et un bouton dans ton navigateur allume la LED. Derrière cette banalité apparente il y a toute la pile réseau : une adresse IP, un socket, un serveur qui écoute, un protocole texte, un client qui interprète.

**Ce que tu apprends.** Modèle client-serveur, protocole HTTP (méthodes, routes, codes de statut), HTML et CSS de base, pile TCP/IP, notion de socket et de port, système de fichiers embarqué (LittleFS) pour ne pas stocker le HTML en dur.

---

### 12 · Portail captif de configuration

**Le projet.** Plus jamais de mot de passe wifi écrit dans le code. L'ESP32 démarre en point d'accès, affiche un formulaire dès qu'on s'y connecte, et enregistre les identifiants en mémoire non volatile. C'est la différence entre un projet d'atelier et un objet qu'on peut donner à quelqu'un.

**Ce que tu apprends.** Mode point d'accès, serveur DNS et redirection captive, formulaires HTML, stockage non volatile (NVS), gestion de configuration séparée du code, hygiène des secrets.

---

### 13 · Mise à jour par le wifi

**Le projet.** Téléverser un nouveau firmware sans toucher au câble. Le jour où ton PAMI sera vissé, câblé et fermé, cette compétence transformera dix minutes de démontage en dix secondes d'attente.

**Ce que tu apprends.** Partitionnement de la flash, notion d'image firmware, mécanisme de rollback en cas d'échec, vérification d'intégrité, protection par mot de passe, culture du déploiement.

---

### 14 · Client MQTT vers ton homelab

**Le projet.** Tu montes un broker Mosquitto en conteneur sur ton serveur, et l'ESP32 y publie ses mesures pendant qu'il s'abonne à des commandes. Le changement de modèle est profond : en HTTP tu demandes, en MQTT on te pousse. Pour de la télémétrie continue, la seconde approche gagne largement.

**Ce que tu apprends.** Modèle publication/abonnement, hiérarchie de topics, qualité de service, testament (last will), reconnexion automatique, architecture découplée, Docker et conteneurs.

---

### 15 · Consommer une API REST en JSON

**Le projet.** Récupérer l'heure exacte par NTP, la météo de ta ville, n'importe quelle donnée publique. Puis parser le JSON dans 512 ko de RAM, ce qui t'oblige à réfléchir à la mémoire pour la première fois.

**Ce que tu apprends.** Architecture REST, verbes et en-têtes HTTP, chiffrement TLS et validation de certificats, sérialisation et désérialisation JSON, allocation statique contre dynamique, gestion d'erreur réseau et timeouts.

---

### 16 · Dashboard temps réel en WebSocket

**Le projet.** Un graphe qui bouge dans ton navigateur au rythme réel du capteur, sans rafraîchir la page. Tu ouvres une connexion permanente et bidirectionnelle, et tu découvres la différence entre interroger et être notifié.

**Ce que tu apprends.** Protocole WebSocket, communication bidirectionnelle persistante, JavaScript côté client, rendu de graphes temps réel, latence contre débit, gestion de la saturation quand le client est plus lent que la source.

---

### 17 · ESP-NOW entre deux cartes

**Le projet.** Deux ESP32 qui se parlent directement, sans box, sans routeur, sans adresse IP, en moins de cinq millisecondes. C'est le protocole de ta future télécommande et de la liaison entre ton robot principal et ton PAMI.

**Ce que tu apprends.** Protocole propriétaire au niveau liaison, adresses MAC, appairage, envoi de structures binaires plutôt que de texte, taille maximale de paquet, portée et puissance d'émission, canaux wifi.

---

### 18 · Réseau un vers plusieurs

**Le projet.** Un maître, plusieurs esclaves, des accusés de réception, un adressage logique. Tu passes d'une liaison point à point à un vrai petit réseau, avec tous les problèmes que ça pose : qui parle quand, que faire quand un message se perd, comment savoir qu'un nœud est mort.

**Ce que tu apprends.** Topologie réseau, diffusion contre envoi ciblé, adressage logique, acquittement et retransmission, détection de perte de nœud, machine à états de communication.

---

### 19 · Manette Bluetooth

**Le projet.** Appairer une manette de console en BLE et lire ses joysticks, ses gâchettes et ses boutons. Tu obtiens une interface de pilotage professionnelle pour zéro effort mécanique, et tu découvres un protocole radicalement différent d'ESP-NOW.

**Ce que tu apprends.** Bluetooth Low Energy, modèle GATT (services et caractéristiques), rapports HID, appairage et liaison persistante, programmation événementielle asynchrone, normalisation des axes et zone morte.

---

### 20 · Trame binaire avec somme de contrôle

**Le projet.** Un protocole maison entre deux microcontrôleurs : octet de début, longueur, charge utile, somme de contrôle. Puis tu débranches un fil en pleine transmission et tu vérifies que le récepteur se resynchronise au lieu de partir en vrille. C'est là que tu passes de « ça marche sur mon bureau » à « ça marche en compétition ».

**Ce que tu apprends.** Liaison UART, structures binaires et alignement mémoire, boutisme, CRC et somme de contrôle, machine à états de réception, robustesse aux erreurs, débogage à l'analyseur logique.

---

# Catégorie 3 — Capteurs et affichage

Un robot sans capteurs est une machine qui exécute aveuglément. Cette catégorie t'apprend à mesurer le monde, à te méfier de tes mesures, et à les rendre lisibles.

---

### 21 · Température et humidité

**Le projet.** Ton premier capteur sur bus I2C. Tu ouvres la datasheet, tu comprends l'adresse du composant, tu lis un registre, tu appliques la formule de conversion, et un nombre devient une grandeur physique. Puis tu affiches la valeur brute à côté de la valeur lissée et tu vois la différence.

**Ce que tu apprends.** Bus I2C (adressage, maître/esclave, acquittement), lecture de registres, lecture de datasheet, conversion en unités physiques, différence entre précision et résolution, cadence d'échantillonnage.

**Matériel.** Module AHT20 + BMP280.

**Piège classique.** Interroger un capteur plus vite que son temps de conversion te renvoie silencieusement la mesure précédente. Tu crois avoir un capteur rapide, tu as un capteur figé.

---

### 22 · Pression et altitude

**Le projet.** Convertir une pression atmosphérique en altitude. Tu poses le capteur sur une table, tu reviens deux heures plus tard, et l'altitude a bougé de cinq mètres sans que rien n'ait bougé. Tu viens de rencontrer la dérive, et tu comprends pourquoi une mesure absolue vaut rarement une mesure relative recalée.

**Ce que tu apprends.** Formule barométrique, référence relative contre absolue, dérive d'un capteur, calibration au démarrage, première intuition de la fusion de capteurs.

**Matériel.** BMP280.

---

### 23 · Écran OLED

**Le projet.** Afficher tes mesures sur un petit écran monochrome. Tu découvres qu'envoyer une image complète sur un bus I2C prend du temps, et que rafraîchir tout l'écran à chaque boucle ralentit ton programme. Tu apprends à ne redessiner que ce qui change.

**Ce que tu apprends.** Notion de framebuffer, bibliothèque graphique, polices bitmap, système de coordonnées écran, coût d'un transfert sur bus lent, mise en page sur surface réduite.

**Matériel.** SSD1306 0,96 pouce.

---

### 24 · Écran TFT et LVGL

**Le projet.** Ta première vraie interface graphique : des boutons qu'on touche, des jauges qui bougent, plusieurs écrans qu'on enchaîne. LVGL est une bibliothèque professionnelle, utilisée dans l'industrie, et c'est la brique de tes projets 51, 52 et 53.

**Ce que tu apprends.** Bus SPI (horloge, données, sélection de puce), double tampon d'affichage, boucle d'événements graphique, système de widgets, gestion du tactile, thèmes et styles, contraintes mémoire — c'est ici que la PSRAM de ta carte sert enfin.

**Matériel.** ST7789 SPI ou carte à écran tactile intégré.

---

### 25 · Télémètre à ultrasons

**Le projet.** Mesurer une distance par le temps de vol d'un écho sonore. Ça marche, c'est spectaculaire, et tu comprends très vite pourquoi tu ne l'utiliseras pas sur ton PAMI : le faisceau est large, la mesure est lente, et le moindre angle renvoie l'écho ailleurs.

**Ce que tu apprends.** Mesure de durée d'impulsion, vitesse du son et dépendance à la température, angle d'ouverture d'un capteur, échos parasites, filtre médian, gestion du timeout, choix d'une technologie par ses limites.

**Matériel.** HC-SR04.

---

### 26 · Télémètre laser à temps de vol

**Le projet.** Le VL53L1X mesure au millimètre par temps de vol optique, avec un faisceau étroit et une cadence élevée. Le vrai exercice est d'en mettre quatre sur le même bus I2C alors qu'ils sortent tous d'usine avec la même adresse — tu apprends à les réveiller un par un pour les renommer.

**Ce que tu apprends.** Temps de vol optique, conflit d'adresse I2C et réassignation logicielle, broche XSHUT, modes de portée courte et longue, budget temporel de mesure, influence de la réflectivité des cibles.

**Matériel.** 3 ou 4 × VL53L1X.

---

### 27 · Capteurs de ligne infrarouges

**Le projet.** Distinguer le noir du blanc par réflexion infrarouge. Tu calibres en balayant le capteur sur les deux surfaces pour mémoriser le minimum et le maximum, puis tu normalises. C'est ta sécurité anti-chute sur le PAMI et la base du projet 36.

**Ce que tu apprends.** Réflexion infrarouge, calibration en deux points, normalisation, seuillage avec hystérésis, sensibilité à la lumière ambiante, importance de la hauteur de montage.

**Matériel.** Barrette 5 capteurs TCRT5000.

---

### 28 · Centrale inertielle

**Le projet.** Un accéléromètre te donne l'orientation mais tremble à la moindre vibration. Un gyroscope est parfaitement lisse mais dérive lentement et sûrement. Tu combines les deux par un filtre complémentaire et tu obtiens un anle à la fois stable et fiable. C'est ta première fusion de capteurs, et c'est un moment marquant.

**Ce que tu apprends.** Accélération contre vitesse angulaire, intégration numérique et dérive, filtre complémentaire, biais et calibration au repos, fusion de capteurs, aperçu des quaternions.

**Matériel.** MPU6050.

**Piège classique.** Intégrer un gyroscope sans corriger son biais fait dériver le cap de plusieurs degrés par minute. Il faut mesurer le biais à l'arrêt, à chaque démarrage.

---

### 29 · Menu à encodeur rotatif

**Le projet.** Naviguer dans des réglages, modifier un gain, choisir une stratégie — sans ordinateur, sans recompiler. Le jour d'un match, changer un paramètre en dix secondes au lieu de sortir le portable change tout.

**Ce que tu apprends.** Décodage en quadrature (version simplifiée), interruptions, structures de menu arborescentes, sauvegarde de paramètres en mémoire non volatile, ergonomie sous contrainte de temps.

**Matériel.** Encodeur KY-040, écran OLED.

---

### 30 · Data logger vers Grafana

**Le projet.** Tes capteurs publient en MQTT, un InfluxDB sur ton homelab stocke, Grafana affiche. En parallèle, une carte SD garde une copie locale au cas où le réseau tombe. Tes données cessent d'être des nombres qui défilent pour devenir un historique exploitable.

**Ce que tu apprends.** Base de données de séries temporelles, horodatage et synchronisation NTP, agrégation et échantillonnage, construction de dashboards, système de fichiers FAT sur carte SD, format CSV, notion de pipeline de données.

**Matériel.** Lecteur de carte SD.

---

# Catégorie 4 — Motorisation et déplacement

Le passage de l'électronique de signal à l'électronique de puissance. C'est aussi ici que se trouvent les deux projets qui conditionnent tout le PAMI.

---

### 31 · Servomoteur et balayage

**Le projet.** Piloter un angle plutôt qu'une vitesse. Un servo contient déjà son propre asservissement : tu lui envoies une consigne, il s'y tient. Tu découvres aussi qu'il tire des pointes de courant qui font redémarrer l'ESP32 s'il partage la même alimentation.

**Ce que tu apprends.** Signal PWM à 50 Hz et largeur d'impulsion, asservissement interne d'un servo, notion de couple, alimentation séparée avec masse commune, appel de courant.

**Matériel.** Servo SG90.

---

### 32 · Moteur DC et pont en H

**Le projet.** Sens, vitesse, freinage, roue libre. Tu comprends physiquement pourquoi un moteur ne se branche jamais sur une sortie logique : il tire cent fois trop de courant et renvoie des surtensions quand il s'arrête. Le pont en H est l'interface qui rend les deux mondes compatibles.

**Ce que tu apprends.** Pont en H, décroissance rapide et lente du courant, courant de démarrage et de calage, force contre-électromotrice, diodes de roue libre, antiparasitage par condensateur, dissipation thermique, soudure de puissance.

**Matériel.** DRV8833, moteur N20.

---

### 33 · Encodeurs en quadrature

**Le projet.** Deux signaux déphasés te disent non seulement de combien l'arbre a tourné, mais dans quel sens. Tu comptes les tics, tu les convertis en millimètres, et ton robot sait enfin où il est allé. Sur ESP32-S3, tu utilises le périphérique matériel PCNT qui compte sans charger le processeur.

**Ce que tu apprends.** Codage en quadrature, décodage x1/x2/x4, interruptions contre périphérique matériel, résolution effective, conversion tics vers distance, atomicité d'une variable partagée avec une interruption.

**Matériel.** Moteurs N20 à encodeur.

---

### 34 · Asservissement PID de vitesse

**Le projet.** Le projet qui fait tout basculer. Tu mesures la vitesse réelle, tu la compares à la consigne, tu corriges. Puis tu règles les trois gains un par un en traçant la réponse. À la fin, ton moteur tient sa vitesse quelle que soit la charge, la pente ou l'usure de la batterie. C'est le premier verrou du PAMI.

**Ce que tu apprends.** Boucle fermée, notion d'erreur, actions proportionnelle, intégrale et dérivée, anti-emballement de l'intégrateur, saturation de commande, période d'échantillonnage constante, réglage empirique, tracé et lecture d'une réponse indicielle, notion de stabilité.

**Matériel.** Moteurs à encodeur, driver.

**Piège classique.** Appeler la boucle PID à une période variable rend les gains intégral et dérivé faux. Le PID doit tourner dans une tâche périodique stricte, pas dans la boucle principale.

---

### 35 · Base deux roues et odométrie

**Le projet.** Deux moteurs, deux encodeurs, une roue folle. Tu demandes « avance d'un mètre » et tu mesures l'écart au réglet. Puis « tourne de 90 degrés » et tu mesures l'angle. Tu calibres l'entraxe et le diamètre des roues jusqu'à ce que l'erreur devienne négligeable. C'est le second verrou du PAMI.

**Ce que tu apprends.** Odométrie différentielle, entraxe et périmètre de roue, intégration de la pose (x, y, thêta), erreurs systématiques contre erreurs aléatoires, protocole de calibration, changement de repère.

**Matériel.** Châssis, roues, roue folle.

---

### 36 · Suiveur de ligne asservi

**Le projet.** Cinq capteurs, une seule erreur calculée par barycentre, et un PID qui la ramène à zéro. Le robot suit la ligne à vitesse croissante jusqu'à ce qu'il décroche, et tu comprends physiquement ce que veut dire « limite de stabilité ». C'est le projet le plus gratifiant de la catégorie.

**Ce que tu apprends.** Fusion de capteurs par barycentre pondéré, normalisation, application d'un PID à une erreur non physique, gestion de la perte de ligne, modulation de la vitesse selon la courbure.

**Matériel.** Barrette IR, base roulante.

---

### 37 · Évitement d'obstacles

**Le projet.** Un télémètre monté sur un servo balaie 180 degrés et construit une carte locale. Le robot choisit la direction la plus dégagée et contourne. Tu passes du réflexe à la décision.

**Ce que tu apprends.** Synchronisation entre capteur et actionneur, grille d'occupation locale, algorithme de contournement réactif, hystérésis de décision pour éviter l'indécision, notion de champ de potentiel.

**Matériel.** VL53L1X, servo.

---

### 38 · Moteur pas à pas

**Le projet.** Une autre philosophie : au lieu d'asservir, tu comptes des pas. Précision absolue tant que tu ne perds pas de pas — et tout l'art consiste à ne pas en perdre. Tu règles le courant de phase, tu construis une rampe d'accélération.

**Ce que tu apprends.** Séquencement de bobines, micro-pas, réglage du courant de phase sur le driver, perte de pas, rampe d'accélération, boucle ouverte contre boucle fermée, gestion thermique.

**Matériel.** NEMA17, TMC2209.

---

### 39 · Bras à trois axes

**Le projet.** Tu donnes une position dans l'espace, le programme calcule les trois angles à appliquer. C'est de la trigonométrie appliquée, et c'est le moment où les mathématiques du lycée deviennent soudain très concrètes.

**Ce que tu apprends.** Cinématique directe et inverse, trigonométrie appliquée, limites articulaires, singularités, interpolation de trajectoire, séquencement de plusieurs actionneurs.

**Matériel.** 3 servos MG996R.

---

### 40 · Télécommande maison

**Le projet.** Deux joysticks, un boîtier imprimé, une liaison ESP-NOW. Mais le vrai exercice n'est pas de faire avancer le robot : c'est de garantir qu'il s'arrête quand la liaison tombe. Un engin qui continue tout droit après une perte de signal est un engin dangereux.

**Ce que tu apprends.** Échantillonnage et zone morte des axes, courbes exponentielles de pilotage, protocole temps réel, chien de garde et détection de perte de liaison, mode dégradé, ergonomie de pilotage.

**Matériel.** 2 joysticks, second ESP32.

---

# Catégorie 5 — Le tout ensemble : ton PAMI

Ici, plus aucun projet n'est isolé. Chacun assemble ce que tu as appris ailleurs sous des contraintes réelles : un règlement, un chronomètre, un adversaire, et un jury d'homologation.

Rappel des contraintes connues : l'ensemble des PAMI d'une équipe doit tenir dans une zone de départ de 150 × 450 mm, chaque PAMI pèse au maximum 1,5 kg, une zone de 30 × 30 mm doit rester disponible pour l'autocollant de stand, et aucun PAMI ne peut être commandé depuis l'extérieur de la table. Le règlement complet de l'édition sort en septembre.

---

### 41 · Châssis imprimé en 3D

**Le projet.** Concevoir sous contrainte réelle : tenir dans le gabarit, rester sous le poids, garder l'accès aux connecteurs, prévoir où passent les câbles, et pouvoir démonter en compétition sans tout défaire. La première version sera trop belle et impossible à réparer. La troisième sera la bonne.

**Ce que tu apprends.** CAO paramétrique, tolérances d'impression et jeux fonctionnels, inserts filetés laiton, choix de matériau (PETG contre PLA), conception pour la maintenance, gestion du volume de câblage, prototypage itératif.

---

### 42 · Alimentation embarquée fiable

**Le projet.** Accu, protection, régulation, mesure de tension embarquée. Beaucoup d'équipes perdent des matchs non pas sur leur algorithme mais sur un rail 5 V qui s'effondre au démarrage des moteurs et fait redémarrer le microcontrôleur au pire moment.

**Ce que tu apprends.** Chimie Li-ion et LiPo, rôle d'un BMS, régulation à découpage contre linéaire, rendement, courant d'appel et condensateurs de réservoir, découplage, chute de tension dans les câbles, dimensionnement de fusible, sécurité batterie.

---

### 43 · Carte de contrôle propre

**Le projet.** Passer du fil volant au fond de panier, puis au circuit imprimé. Tu dessines un schéma, tu routes, tu fais fabriquer, tu assembles. Ton robot cesse d'être un prototype qui se débranche au transport pour devenir un produit.

**Ce que tu apprends.** Lecture et écriture de schéma électronique, routage, plan de masse, découplage, choix de connecteurs à détrompeur, sérigraphie utile, CAO électronique avec KiCad, chaîne de fabrication et d'assemblage, soudure fine.

---

### 44 · Odométrie calibrée

**Le projet.** Écrire un protocole de calibration reproductible, le faire tourner dix fois, mesurer la dispersion. Tu quittes le bricolage pour la méthode expérimentale, et tu obtiens un chiffre : ton erreur est de tant de millimètres par mètre parcouru.

**Ce que tu apprends.** Méthode expérimentale, correction d'erreurs systématiques, statistiques simples (moyenne, écart-type), reproductibilité, rédaction d'une procédure utilisable par un coéquipier.

---

### 45 · Asservissement en position

**Le projet.** Deux boucles imbriquées, une pour la distance et une pour l'angle, alimentées par un profil de vitesse trapézoïdal qui accélère, roule et freine proprement. Le robot arrive au point demandé sans dépasser, sans osciller, sans trembler.

**Ce que tu apprends.** Génération de trajectoire, profil trapézoïdal et en S, découplage des axes, saturation coordonnée de deux commandes, critère d'arrivée (fenêtre de tolérance plus temps de stabilisation), détection de blocage mécanique.

---

### 46 · Détection d'adversaire

**Le projet.** Une couronne de télémètres, une zone d'arrêt dont la taille dépend de la vitesse, un arrêt franc et une reprise automatique quand la voie se libère. C'est une exigence d'homologation, pas une option.

**Ce que tu apprends.** Fusion multi-capteurs, zones de sécurité dynamiques, machine à états de sécurité prioritaire sur la mission, temps de réaction, arbitrage entre faux positifs et faux négatifs, conformité à un règlement.

---

### 47 · Recalage sur bordures

**Le projet.** L'odométrie dérive, c'est une certitude mathématique. La parade est d'aller toucher un élément dont la position est connue pour remettre l'estimation à zéro. Une seconde de recalage efface trente secondes d'erreur accumulée.

**Ce que tu apprends.** Notion de recalage, fusion d'une estimation relative et d'une mesure absolue, introduction au filtre de Kalman, contacteurs de fin de course, séquence de recalage, représentation de l'incertitude.

---

### 48 · Navigation par points de passage

**Le projet.** Une mission devient une liste de points à atteindre dans l'ordre, avec des actions à chaque étape. Quand un adversaire bloque le chemin, le robot ne s'obstine pas : il replanifie ou passe à l'objectif suivant.

**Ce que tu apprends.** Liste de points de passage, machine à états de mission, replanification, algorithmes de recherche de chemin (A*, champs de potentiel), gestion des délais, priorisation dynamique des objectifs.

---

### 49 · Machine à états de match

**Le projet.** Tirette de départ, choix de stratégie, chronomètre strict, arrêt propre à la fin du temps réglementaire. Zéro comportement imprévu, zéro état non prévu, zéro surprise devant le jury. C'est le projet le moins spectaculaire et le plus déterminant.

**Ce que tu apprends.** Hiérarchie de machines à états, contraintes temps réel dures, chien de garde, séquence d'initialisation et vérifications au démarrage, arrêt d'urgence matériel et logiciel, tests de non-régression, préparation à l'homologation.

---

### 50 · Télémétrie et rejeu de match

**Le projet.** Enregistrer la trajectoire, les consignes, les mesures et les décisions pendant tout le match, puis les rejouer tranquillement après coup. Tu arrêtes de deviner pourquoi le robot s'est arrêté au troisième point : tu le vois.

**Ce que tu apprends.** Format de log binaire compact, horodatage cohérent, transmission sans fil pendant le match, outillage de visualisation (Python et matplotlib, ou Grafana), débogage post-mortem, culture de la mesure plutôt que de l'intuition.

---

# Catégorie 6 — Tes projets personnels

Ils ne sont pas plus faciles que les autres. Ils dépendent simplement de briques acquises ailleurs, indiquées en prérequis.

---

### 51 · StreamDeck USB HID

**Le projet.** Une matrice de touches mécaniques, un écran de contexte, et l'ESP32-S3 reconnu par ton Fedora comme un clavier natif — sans puce d'interface, grâce à l'USB intégré de la puce. Chaque touche déclenche une macro, et l'écran affiche le profil actif.

**Ce que tu apprends.** USB HID et descripteurs de rapport, matrice de touches et diodes anti-fantôme, anti-rebond appliqué à quinze contacts, gestion de profils applicatifs, intégration système sous Linux (règles udev), conception de boîtier imprimé.

**Prérequis.** Projets 02, 05 et 24.

---

### 52 · Panneau mural Home Assistant

**Le projet.** Un écran tactile abonné à tes capteurs de température, qui affiche l'état de la maison et permet d'agir. Il doit rester lisible, se mettre en veille, et surtout ne pas afficher n'importe quoi quand le réseau tombe.

**Ce que tu apprends.** Intégration domotique (découverte MQTT ou ESPHome), design d'interface tactile, mise en veille et réveil, alimentation permanente propre, montage mural, gestion explicite de l'indisponibilité réseau.

**Prérequis.** Projets 14, 24 et 30.

---

### 53 · Contrôleur de LED de chambre

**Le projet.** Un bandeau adressable piloté depuis un panneau tactile mural : couleur, intensité, scènes, transitions douces. Tu réutilises directement la conversion HSV du projet 06 et l'interface du projet 24.

**Ce que tu apprends.** Protocole WS2812 et timing critique (périphérique RMT de l'ESP32), gestion d'un courant élevé, injection d'alimentation en plusieurs points du bandeau, courbes de transition, température de couleur, intégration à ta domotique.

**Prérequis.** Projets 06, 24 et 52.

**Piège classique.** Cinq mètres de WS2812B en blanc plein tirent près de 15 A. Le dimensionnement de l'alimentation et l'injection en plusieurs points ne sont pas des raffinements, ce sont des conditions de fonctionnement.

---

### 54 · Voiture radiocommandée

**Le projet.** Châssis imprimé, direction à servo, propulsion à moteur DC, télécommande ESP-NOW. C'est l'assemblage direct des projets 31, 32 et 40, avec en plus toute la partie mécanique.

**Ce que tu apprends.** Architecture de châssis, géométrie de direction d'Ackermann, rapport de transmission, position du centre de gravité, télémétrie de retour vers la télécommande, failsafe appliqué à un engin rapide.

**Prérequis.** Projets 31, 32 et 40.

---

### 55 · Avion radiocommandé

**Le projet.** Aile imprimée, moteur brushless, quatre servos de gouverne. Le vol est impitoyable : une erreur de centrage et l'appareil ne décolle pas ou ne se rattrape pas. C'est le projet le plus exigeant de la liste sur le plan mécanique.

**Ce que tu apprends.** Aérodynamique de base (portance, centrage, dièdre), moteur brushless et variateur électronique, calage et équilibrage d'hélice, mixage des gouvernes, sécurité LiPo en vol, réglementation aéromodélisme, portée radio et passage à ExpressLRS.

**Prérequis.** Projets 31, 40 et 42.

**Piège classique.** Le centrage est ce qui fait voler ou s'écraser un avion. Avant le premier vol, vérifie-le, puis fais-le vérifier par quelqu'un d'autre.

---

# Annexe A — Index des technologies

Toutes les compétences de la feuille de route, regroupées par domaine, avec les projets où elles apparaissent. C'est la liste à relire quand tu te demandes ce que tu sais déjà faire.

## Électronique et travail manuel

| Compétence | Projets |
|---|---|
| Soudure de composants traversants | 32, 41, 43 |
| Soudure fine et de puissance | 42, 43 |
| Lecture de datasheet | 21, 26, 32, 38 |
| Usage du multimètre (tension, continuité) | 04, 10, 42 |
| Analyseur logique | 20, 21, 26 |
| Pont diviseur et mise à l'échelle d'une tension | 42 |
| Découplage et condensateurs de réservoir | 32, 42, 53 |
| Régulation à découpage et rendement | 42 |
| Antiparasitage d'un moteur | 32, 42 |
| Sécurité batterie LiPo | 42, 55 |
| CAO électronique et routage | 43 |

## Programmation en C et architecture logicielle

| Compétence | Projets |
|---|---|
| Machine à états finis | 01, 08, 18, 46, 48, 49 |
| Modularité et compilation séparée | 08, et tout ce qui suit |
| Pointeurs de fonction et tables de dispatch | 05 |
| Structures binaires, alignement, boutisme | 17, 20, 50 |
| Gestion mémoire, statique contre dynamique | 15, 24 |
| Arithmétique entière et virgule flottante | 03, 06, 34 |
| Conception d'API réutilisable | 02, 05, 06, 08 |
| Débogage systématique et instrumentation | 05, 34, 50 |

## Systèmes embarqués

| Compétence | Projets |
|---|---|
| Temps non bloquant | 01 |
| PWM matériel (LEDC) | 03, 31, 32 |
| Convertisseur analogique-numérique | 04, 42 |
| Compteur d'impulsions matériel (PCNT) | 33 |
| Générateur de signal RMT | 53 |
| FreeRTOS, tâches, files, mutex | 09, 34 |
| Interruptions et atomicité | 29, 33 |
| Modes de sommeil et autonomie | 10 |
| Stockage non volatile (NVS) | 12, 29 |
| Système de fichiers embarqué | 11, 30 |
| Mise à jour à distance (OTA) | 13 |
| Chien de garde | 40, 49 |

## Bus et protocoles

| Compétence | Projets |
|---|---|
| UART | 05, 20 |
| I2C, adressage et conflits | 21, 26 |
| SPI | 24, 30 |
| Somme de contrôle et CRC | 20 |
| JSON | 15 |
| HTTP et REST | 11, 15 |
| MQTT, modèle publication/abonnement | 14, 30, 52 |
| WebSocket | 16 |
| ESP-NOW | 17, 18, 40, 50 |
| Bluetooth Low Energy et HID | 19 |
| USB HID | 51 |
| WS2812 et timing critique | 53 |

## Réseau, serveurs et outillage

| Compétence | Projets |
|---|---|
| Modèle client-serveur | 11, 14, 16 |
| Pile TCP/IP, adressage, sockets | 11 |
| TLS et certificats | 15 |
| Docker et services auto-hébergés | 14, 30 |
| Base de séries temporelles (InfluxDB) | 30 |
| Dashboards (Grafana) | 30, 50 |
| HTML, CSS, JavaScript de base | 11, 16 |
| Intégration domotique | 52, 53 |

## Automatique et robotique

| Compétence | Projets |
|---|---|
| Boucle ouverte contre boucle fermée | 32, 34, 38 |
| Régulateur PID et réglage des gains | 34, 36, 45 |
| Filtrage du bruit de mesure | 04, 25, 28 |
| Odométrie et estimation de pose | 35, 44 |
| Décodage en quadrature | 29, 33 |
| Fusion de capteurs | 28, 36, 46, 47 |
| Filtre complémentaire | 28 |
| Introduction au filtre de Kalman | 47 |
| Génération de trajectoire | 38, 45 |
| Cinématique inverse | 39 |
| Planification de chemin et replanification | 37, 48 |
| Zones de sécurité et arrêt d'urgence | 46, 49 |

## Mécanique et fabrication

| Compétence | Projets |
|---|---|
| CAO paramétrique | 41, 54, 55 |
| Impression 3D, tolérances et jeux | 41 |
| Inserts filetés et assemblage démontable | 41 |
| Transmission et rapport de réduction | 33, 54 |
| Centre de gravité et équilibrage | 54, 55 |
| Aérodynamique de base | 55 |

## Méthode et pratique d'ingénieur

| Compétence | Projets |
|---|---|
| Calibration et protocole reproductible | 27, 28, 35, 44 |
| Statistiques de mesure | 44 |
| Journalisation et analyse post-mortem | 50 |
| Documentation d'une procédure | 44, 49 |
| Tests de non-régression | 49 |
| Conception sous contrainte réglementaire | 41, 46, 49 |
| Travail en équipe et transmission | 43, 44 |

---

# Annexe B — Jalons proposés

Le règlement de l'édition paraît en septembre. En calant la progression sur une coupe qui se joue au printemps :

| Période | Projets | Objectif |
|---|---|---|
| Août – septembre | 01 – 20 | Les réflexes de base et la communication |
| Octobre – novembre | 21 – 33 | Capteurs, affichage, premiers moteurs |
| Décembre – janvier | **34 – 35** | Les deux verrous : PID et odométrie |
| Février | 36 – 45 | Base roulante complète et châssis |
| Mars | 46 – 49 | Sécurité, stratégie, homologation |
| Avril | 50 | Télémétrie, réglages fins, répétitions |

Les projets 51 à 55 se glissent entre les autres, quand tu as besoin de souffler. Ils ne sont pas une distraction : le StreamDeck te fait maîtriser LVGL, qui te servira pour l'interface de réglage du PAMI.

---

# Annexe C — Avant de commencer

Trois principes qui feront la différence sur l'ensemble du parcours.

**Écris chaque module comme si tu allais le réutiliser.** Un fichier `.c` et un `.h` par fonction, une interface publique minimale, aucune dépendance cachée. À la fin de la liste tu auras une bibliothèque personnelle, et l'équipe de l'an prochain repartira de ton code au lieu de tout réécrire.

**Mesure au lieu de deviner.** Le multimètre, l'analyseur logique et le journal de bord valent mieux que des heures d'hypothèses. La plupart des bugs de robotique sont électriques, pas logiciels.

**Note tout ce qui t'a coûté plus de trente minutes.** Un fichier de notes par projet, avec le symptôme et la cause réelle. C'est ce document, dans six mois, qui te fera gagner la compétition.
