#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// --- 1. CONFIGURATION RÉSEAU ET MQTT ---
const char* ssid = "VOTRE_WIFI";            
const char* password = "VOTRE_MOT_DE_PASSE";
const char* mqtt_server = "192.168.1.XX";   // <--- IP de votre PC (Gateway)
const char* mqtt_topic = "building/lab/node_01/telemetry";
const char* node_id = "node_01";


#define MQ_PIN 34       // ADC1 (Safe avec Wi-Fi). Branchez la sortie A0 du capteur ici.
#define FLAME_PIN 35    // Input Only. Branchez la sortie D0 du capteur ici.
#define DHTPIN 4        // Digital I/O.
#define DHTTYPE DHT11   // Mettez DHT22 si votre capteur est blanc

// --- 3. OBJETS ---
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

// --- 4. FONCTIONS ---

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion au Wi-Fi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n Wi-Fi Connecté !");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connexion au Broker MQTT...");

    // ID Client Unique pour éviter les déconnexions
    if (client.connect("NodeMCU32S_FireSystem")) {
      Serial.println("Connecté !");
    } else {
      Serial.print("Echec, rc=");
      Serial.print(client.state());
      Serial.println(" nouvelle tentative dans 5s");
      delay(5000);
    }
  }
}

// --- 5. SETUP ---
void setup() {
  Serial.begin(115200);
  
  // Initialisation Capteurs
  dht.begin();
  
  // Le NodeMCU nécessite parfois de définir explicitement le mode INPUT
  pinMode(MQ_PIN, INPUT);
  pinMode(FLAME_PIN, INPUT); 

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

// --- 6. LOOP ---
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lecture des capteurs
  int raw_gas = analogRead(MQ_PIN); // Lecture 0-4095
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int flame_detected = digitalRead(FLAME_PIN); 

  // Vérification des erreurs de lecture (Typique sur DHT)
  if (isnan(temp) || isnan(hum)) {
    Serial.println(" Erreur lecture DHT ! Vérifiez le câble.");
    // On continue quand même pour envoyer les autres données
    temp = 0.0; 
    hum = 0.0;
  }

  // Création du JSON
  StaticJsonDocument<256> doc;
  doc["node_id"] = node_id;
  doc["raw_gas"] = raw_gas;
  doc["temp"] = temp;
  doc["humidity"] = hum;
  
  // Note: Souvent les capteurs flamme envoient 0 quand il y a du feu (LOW active)
  // On inverse la logique ici pour que 1 = FEU dans la base de données
  doc["flame_dig"] = (flame_detected == LOW) ? 1 : 0; 

  char buffer[512];
  serializeJson(doc, buffer);

  // Envoi
  Serial.print("Envoi: ");
  Serial.println(buffer);
  client.publish(mqtt_topic, buffer);

  delay(2000); 
}