/* ===========================================================================
 * Portail captif de configuration Wi-Fi — ESP32-S3
 * A placer dans src/main.cpp
 *
 * PRINCIPE
 *   Au demarrage, la carte lit ses identifiants Wi-Fi en memoire NVS.
 *     - Trouves et valides  -> elle se connecte (LED verte).
 *     - Absents ou refuses  -> elle ouvre son propre reseau "ESP32-Config"
 *                              et sert une page de configuration (LED bleue).
 *   Tu t'y connectes avec un telephone, la page s'ouvre toute seule, tu
 *   choisis ton reseau, tu saisis le mot de passe : la carte redemarre
 *   connectee. Les identifiants survivent aux coupures de courant ET aux
 *   reflashages.
 *
 * CODES COULEUR
 *   Bleu clignotant .... recherche des reseaux
 *   Bleu fixe .......... portail ouvert, en attente de configuration
 *   Ambre clignotant ... tentative de connexion
 *   Vert fixe .......... connecte
 *   Rouge .............. effacement des identifiants en cours
 *
 * REINITIALISATION
 *   Maintenir le bouton BOOT 3 secondes efface la NVS et redemarre.
 * ======================================================================== */

 #include <Arduino.h>
 #include <WiFi.h>
 #include <WebServer.h>
 #include <DNSServer.h>
 #include <Preferences.h>
 
 #include <vector>
 #include <algorithm>
 
 /* ---------------------------------------------------------------------------
  * 1. Configuration
  * ------------------------------------------------------------------------ */
 
 // constexpr : evalue a la COMPILATION. Contrairement a une variable globale,
 // ces valeurs ne consomment aucune RAM a l'execution.
 constexpr char     AP_SSID[]           = "ESP32-Config";
 constexpr char     AP_PASS[]           = "";      // vide = reseau ouvert
 constexpr uint16_t DNS_PORT            = 53;
 constexpr uint32_t CONNECT_TIMEOUT_MS  = 15000;   // abandon apres 15 s
 constexpr int      BTN_RESET           = 0;       // GPIO 0 = bouton BOOT
 constexpr uint32_t RESET_HOLD_MS       = 3000;    // appui long = effacement
 
 // Espace de noms NVS. Deux projets differents peuvent utiliser la meme cle
 // "ssid" sans se marcher dessus tant que leurs namespaces different.
 constexpr char NVS_NAMESPACE[] = "wifi";
 constexpr char NVS_KEY_SSID[]  = "ssid";
 constexpr char NVS_KEY_PASS[]  = "pass";
 
 /* ---------------------------------------------------------------------------
  * 2. Types et etat global
  * ------------------------------------------------------------------------ */
 
 // enum class plutot que enum : les valeurs sont typees et ne polluent pas
 // l'espace de noms global. Il faut ecrire Etat::Portail, pas juste Portail.
 enum class Etat : uint8_t { Connecte, Portail };
 
 struct Identifiants {
   String ssid;
   String pass;
   bool valide() const { return ssid.length() > 0; }
 };
 
 struct Reseau {
   String  ssid;
   int32_t rssi;      // puissance en dBm : -40 excellent, -90 inutilisable
   bool    protege;
 };
 
 WebServer   server(80);
 DNSServer   dns;
 Preferences prefs;
 
 Etat                 etat = Etat::Portail;
 std::vector<Reseau>  reseaux;
 
 /* ---------------------------------------------------------------------------
  * 3. Utilitaires
  * ------------------------------------------------------------------------ */
 
 void led(uint8_t r, uint8_t g, uint8_t b) {
   rgbLedWrite(RGB_BUILTIN, r, g, b);
 }
 
 // Un SSID peut contenir <, > ou " (rien ne l'interdit). Sans echappement, un
 // reseau nomme  <script>  casserait la page. Regle generale : toute donnee
 // exterieure injectee dans du HTML doit etre echappee.
 String echapper(const String &src) {
   String out;
   out.reserve(src.length() + 16);   // evite les reallocations successives
   for (size_t i = 0; i < src.length(); ++i) {
     const char c = src[i];
     switch (c) {
       case '&':  out += "&amp;";  break;
       case '<':  out += "&lt;";   break;
       case '>':  out += "&gt;";   break;
       case '"':  out += "&quot;"; break;
       case '\'': out += "&#39;";  break;
       default:   out += c;
     }
   }
   return out;
 }
 
 /* ---------------------------------------------------------------------------
  * 4. Memoire NVS
  *
  * La NVS est une partition dediee de la flash, geree par ESP-IDF. Elle
  * repartit l'usure sur les secteurs et survit a un "pio run -t upload"
  * ordinaire, contrairement au reste du firmware qui est ecrase.
  * ------------------------------------------------------------------------ */
 
 Identifiants chargerIdentifiants() {
   Identifiants id;
   prefs.begin(NVS_NAMESPACE, true);            // true = lecture seule
   id.ssid = prefs.getString(NVS_KEY_SSID, ""); // "" = valeur par defaut
   id.pass = prefs.getString(NVS_KEY_PASS, "");
   prefs.end();                                 // libere le handle NVS
   return id;
 }
 
 void sauverIdentifiants(const String &ssid, const String &pass) {
   prefs.begin(NVS_NAMESPACE, false);           // false = lecture / ecriture
   prefs.putString(NVS_KEY_SSID, ssid);
   prefs.putString(NVS_KEY_PASS, pass);
   prefs.end();
 }
 
 void effacerIdentifiants() {
   prefs.begin(NVS_NAMESPACE, false);
   prefs.clear();                               // vide tout le namespace
   prefs.end();
 }
 
 /* ---------------------------------------------------------------------------
  * 5. Scan des reseaux
  * ------------------------------------------------------------------------ */
 
 void scanner() {
   Serial.println("Scan en cours...");
   led(0, 0, 40);
 
   reseaux.clear();
   const int n = WiFi.scanNetworks();
   if (n <= 0) { Serial.println("Aucun reseau trouve."); return; }
 
   // reserve() alloue la capacite en une fois. Sans lui, push_back reallouerait
   // et recopierait le tableau a chaque agrandissement : sur un MCU avec
   // 320 Ko de RAM, ces reallocations fragmentent le tas.
   reseaux.reserve(n);
 
   for (int i = 0; i < n; ++i) {
     if (WiFi.SSID(i).isEmpty()) continue;      // ignore les reseaux caches
     reseaux.push_back({ WiFi.SSID(i),
                         WiFi.RSSI(i),
                         WiFi.encryptionType(i) != WIFI_AUTH_OPEN });
   }
   WiFi.scanDelete();                           // libere le buffer du driver
 
   // Un meme SSID apparait souvent plusieurs fois (repeteurs, bi-bande).
   // std::unique ne supprime que les doublons ADJACENTS : il faut donc trier
   // par nom d'abord, dedupliquer, puis retrier par puissance.
   std::sort(reseaux.begin(), reseaux.end(), [](const Reseau &a, const Reseau &b) {
     if (a.ssid != b.ssid) return a.ssid < b.ssid;
     return a.rssi > b.rssi;                    // garde le plus puissant
   });
 
   reseaux.erase(std::unique(reseaux.begin(), reseaux.end(),
                             [](const Reseau &a, const Reseau &b) {
                               return a.ssid == b.ssid;
                             }),
                 reseaux.end());                // idiome erase-remove
 
   std::sort(reseaux.begin(), reseaux.end(),
             [](const Reseau &a, const Reseau &b) { return a.rssi > b.rssi; });
 
   Serial.printf("%u reseaux uniques.\n", (unsigned)reseaux.size());
 }
 
 /* ---------------------------------------------------------------------------
  * 6. Pages HTML
  * ------------------------------------------------------------------------ */
 
 // R"HTML( ... )HTML" est un raw string literal : pas besoin d'echapper les
 // guillemets ni les antislashs. Le delimiteur HTML evite toute collision.
 String pagePortail() {
   String html = R"HTML(<!DOCTYPE html><html lang="fr"><head>
 <meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
 <title>Configuration Wi-Fi</title><style>
 body{font-family:system-ui,-apple-system,sans-serif;max-width:420px;margin:2rem auto;
 padding:0 1rem;background:#f5f5f4;color:#1c1917}
 h1{font-size:1.25rem}
 label{display:block;margin:1.25rem 0 .35rem;font-size:.875rem;color:#57534e}
 select,input{width:100%;padding:.6rem;font-size:1rem;border:1px solid #d6d3d1;
 border-radius:6px;box-sizing:border-box;background:#fff}
 button{width:100%;margin-top:1.75rem;padding:.8rem;font-size:1rem;background:#1c1917;
 color:#fff;border:0;border-radius:6px}
 a{display:inline-block;margin-top:1.25rem;font-size:.8125rem;color:#57534e}
 </style></head><body>
 <h1>Configuration Wi-Fi</h1>
 <form method="POST" action="/sauver">
 <label for="ssid">Réseau détecté</label>
 <select id="ssid" name="ssid">)HTML";
 
   for (const Reseau &r : reseaux) {
     const String nom = echapper(r.ssid);
     html += "<option value=\"" + nom + "\">" + nom;
     html += r.protege ? " — protégé " : " — ouvert ";
     html += "(" + String(r.rssi) + " dBm)</option>";
   }
 
   html += R"HTML(<option value="">— saisir manuellement —</option>
 </select>
 <label for="ssid_manuel">Ou nom du réseau</label>
 <input id="ssid_manuel" name="ssid_manuel" placeholder="MonReseau" autocomplete="off">
 <label for="pass">Mot de passe</label>
 <input id="pass" name="pass" type="password" autocomplete="off">
 <button type="submit">Enregistrer et redémarrer</button>
 </form>
 <a href="/rescan">Relancer la recherche</a>
 </body></html>)HTML";
 
   return html;
 }
 
 String pageMessage(const String &titre, const String &corps) {
   return String(R"HTML(<!DOCTYPE html><html lang="fr"><head><meta charset="utf-8">
 <meta name="viewport" content="width=device-width,initial-scale=1"><style>
 body{font-family:system-ui,sans-serif;max-width:420px;margin:3rem auto;padding:0 1rem;
 color:#1c1917}h1{font-size:1.25rem}
 </style></head><body><h1>)HTML") + titre + "</h1>" + corps + "</body></html>";
 }
 
 /* ---------------------------------------------------------------------------
  * 7. Handlers HTTP
  * ------------------------------------------------------------------------ */
 
 void handleRacine() {
   server.send(200, "text/html; charset=utf-8", pagePortail());
 }
 
 void handleRescan() {
   scanner();
   server.sendHeader("Location", "/", true);
   server.send(302, "text/plain", "");
 }
 
 void handleSauver() {
   String ssid = server.arg("ssid");
   if (ssid.isEmpty()) ssid = server.arg("ssid_manuel");  // repli sur la saisie
   ssid.trim();
   const String pass = server.arg("pass");
 
   if (ssid.isEmpty()) {
     server.send(400, "text/html; charset=utf-8",
                 pageMessage("Nom de réseau vide",
                             "<p><a href=\"/\">Retour</a></p>"));
     return;
   }
 
   sauverIdentifiants(ssid, pass);
   Serial.printf("Identifiants enregistres pour \"%s\".\n", ssid.c_str());
 
   server.send(200, "text/html; charset=utf-8",
               pageMessage("Enregistré",
                           "<p>La carte redémarre et tente de se connecter à <b>"
                           + echapper(ssid) +
                           "</b>.</p><p>La LED passe au vert en cas de succès, "
                           "et le portail se rouvre en cas d'échec.</p>"));
 
   delay(1500);        // laisse le temps a la reponse de partir
   ESP.restart();
 }
 
 // Page servie une fois la carte connectee, sur son IP locale.
 void handleStatut() {
   String c = "<p>Réseau : <b>" + echapper(WiFi.SSID()) + "</b></p>";
   c += "<p>IP : " + WiFi.localIP().toString() + "</p>";
   c += "<p>Signal : " + String(WiFi.RSSI()) + " dBm</p>";
   c += "<p>RAM libre : " + String(ESP.getFreeHeap()) + " octets</p>";
   c += "<p>Uptime : " + String(millis() / 1000) + " s</p>";
   server.send(200, "text/html; charset=utf-8", pageMessage("ESP32-S3 connecté", c));
 }
 
 // LE COEUR DU PORTAIL CAPTIF.
 // Le serveur DNS repond l'IP de la carte a TOUTE requete de nom. Le telephone
 // interroge alors une URL de test (connectivitycheck.gstatic.com sur Android,
 // captive.apple.com sur iOS) et atterrit ici. La redirection 302 declenche
 // l'ouverture automatique de la page de configuration.
 void handleNotFound() {
   server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/", true);
   server.send(302, "text/plain", "");
 }
 
 /* ---------------------------------------------------------------------------
  * 8. Modes de fonctionnement
  * ------------------------------------------------------------------------ */
 
 void demarrerPortail() {
   etat = Etat::Portail;
 
   // WIFI_AP_STA et non WIFI_AP : le mode station reste actif en parallele,
   // c'est lui qui permet de scanner les reseaux pendant que l'AP tourne.
   WiFi.mode(WIFI_AP_STA);
   WiFi.softAP(AP_SSID, strlen(AP_PASS) ? AP_PASS : nullptr);
   delay(100);
 
   const IPAddress ip = WiFi.softAPIP();
   Serial.printf("Portail ouvert. Reseau \"%s\", http://%s\n",
                 AP_SSID, ip.toString().c_str());
 
   scanner();
 
   dns.setErrorReplyCode(DNSReplyCode::NoError);
   dns.start(DNS_PORT, "*", ip);           // "*" : intercepte tous les domaines
 
   server.on("/", HTTP_GET, handleRacine);
   server.on("/rescan", HTTP_GET, handleRescan);
   server.on("/sauver", HTTP_POST, handleSauver);
   server.onNotFound(handleNotFound);
   server.begin();
 
   led(0, 0, 40);                          // bleu fixe = en attente
 }
 
 
 bool connecter(const Identifiants &id) {
   Serial.printf("Connexion a \"%s\"...\n", id.ssid.c_str());
   WiFi.mode(WIFI_STA);
   WiFi.begin(id.ssid.c_str(), id.pass.c_str());
 
   const uint32_t debut = millis();
   while (WiFi.status() != WL_CONNECTED && millis() - debut < CONNECT_TIMEOUT_MS) {
     led(30, 15, 0); delay(200);           // ambre clignotant
     led(0, 0, 0);   delay(200);
   }
   return WiFi.status() == WL_CONNECTED;
 }
 
 void verifierBoutonReset() {
   // static dans une fonction : la variable garde sa valeur entre les appels,
   // sans etre visible depuis l'exterieur. Ideal pour un etat local persistant.
   static uint32_t debutAppui = 0;
 
   if (digitalRead(BTN_RESET) == LOW) {          // LOW = enfonce (INPUT_PULLUP)
     if (debutAppui == 0) {
       debutAppui = millis();
     } else if (millis() - debutAppui > RESET_HOLD_MS) {
       Serial.println("Effacement des identifiants.");
       led(40, 0, 0);
       effacerIdentifiants();
       delay(600);
       ESP.restart();
     }
   } else {
     debutAppui = 0;                             // relache : on repart de zero
   }
 }
 
 /* ---------------------------------------------------------------------------
  * 9. setup / loop
  * ------------------------------------------------------------------------ */
 
 void setup() {
   Serial.begin(115200);
   delay(600);                       // laisse le CDC USB s'enumerer avant d'ecrire
   pinMode(BTN_RESET, INPUT_PULLUP);
   led(0, 0, 0);
 
   Serial.println("\n=== Portail captif Wi-Fi ===");
 
   const Identifiants id = chargerIdentifiants();
 
   if (id.valide() && connecter(id)) {
     etat = Etat::Connecte;
     led(0, 40, 0);
     Serial.printf("Connecte. Ouvre http://%s\n", WiFi.localIP().toString().c_str());
     server.on("/", HTTP_GET, handleStatut);
     server.begin();
   } else {
     Serial.println(id.valide() ? "Echec de connexion." : "Aucun identifiant en NVS.");
     demarrerPortail();
   }
 }
 
 void loop() {
   if (etat == Etat::Portail) {
     dns.processNextRequest();       // doit tourner en continu, sinon pas de portail
     server.handleClient();
   } else {
     server.handleClient();
 
     // Perte de connexion : on rouvre le portail plutot que de rester muet.
     static uint32_t dernierTest = 0;
     if (millis() - dernierTest > 5000) {
       dernierTest = millis();
       if (WiFi.status() != WL_CONNECTED) {
         Serial.println("Connexion perdue, reouverture du portail.");
         demarrerPortail();
       }
     }
   }
 
   verifierBoutonReset();
 }