1. URL de l'API et Accès

yamlAPI_BASE_URL: http://192.168.1.100:5000/api
BACKEND_MONITORING_URL: http://192.168.1.100:5000 (actuellement)

# Accès à la VM
IP: 192.168.1.100
User: me
Port SSH: 22

# Compte de test
Username: admin
Password: admin

DOCUMENTATION API COMPLÈTE


A. Authentification
Login
httpPOST /api/auth/login
Content-Type: application/json

Body:
{
  "username": "admin",
  "password": "admin"
}


Response 200:
{
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "user": {
    "id": 1,
    "username": "admin",
    "email": "admin@iot.local",
    "role": "admin"
  }
}

Vérifier Token
httpGET /api/auth/verify
Authorization: Bearer {token}

Response 200:
{
  "valid": true,
  "user": {
    "user_id": 1,
    "username": "admin",
    "role": "admin"
  }
}
Refresh Token
httpPOST /api/auth/refresh
Authorization: Bearer {token}

Response 200:
{
  "token": "nouveau_token...",
  "message": "Token rafraîchi"
}

B. Capteurs
Liste des capteurs
httpGET /api/capteurs
GET /api/capteurs?actif=true
GET /api/capteurs?type=temperature
Authorization: Bearer {token}

Response 200:
[
  {
    "id": 10,
    "nom": "DHT11 Température",
    "type": "temperature",
    "unite": "°C",
    "description": "Capteur de température",
    "actif": true,
    "date_creation": "2024-12-01T10:00:00",
    "date_modification": "2024-12-01T10:00:00"
  },
  {
    "id": 11,
    "nom": "DHT11 Humidité",
    "type": "humidite",
    "unite": "%",
    "description": "Capteur d'humidité",
    "actif": true,
    "date_creation": "2024-12-01T10:00:00",
    "date_modification": "2024-12-01T10:00:00"
  }
]
Détails d'un capteur
httpGET /api/capteurs/{id}
Authorization: Bearer {token}

Response 200:
{
  "id": 10,
  "nom": "DHT11 Température",
  "type": "temperature",
  "unite": "°C",
  "description": "Capteur de température",
  "actif": true,
  "date_creation": "2024-12-01T10:00:00"
}
Créer un capteur
httpPOST /api/capteurs
Authorization: Bearer {token}
Content-Type: application/json

Body:
{
  "nom": "Nouveau Capteur",
  "type": "temperature",
  "unite": "°C",
  "description": "Description"
}

Response 201:
{
  "message": "Capteur ajouté",
  "id": 15
}

C. Noeuds (ESP32)
Liste des noeuds
httpGET /api/noeuds
GET /api/noeuds?statut=actif
Authorization: Bearer {token}

Response 200:
[
  {
    "id": 4,
    "nom": "NodeMCU-ESP32-Test",
    "adresse_mac": "AA:BB:CC:DD:EE:FF",
    "adresse_ip": "192.168.1.150",
    "localisation": "......",
    "modele": "NodeMCU ESP32-S",
    "firmware_version": "v1.0",
    "statut": "actif",
    "derniere_connexion": "2024-12-07T01:40:00",
    "api_key": "esp32_test_k...",  // Tronquée
    "date_creation": "2024-12-01T10:00:00"
  }
]
Détails d'un noeud
httpGET /api/noeuds/{id}
Authorization: Bearer {token}

Response 200:
{
  "id": 4,
  "nom": "NodeMCU-ESP32-Test",
  "adresse_mac": "AA:BB:CC:DD:EE:FF",
  "adresse_ip": "192.168.1.150",
  "localisation": ".......",
  "modele": "NodeMCU ESP32-S",
  "capteurs": "DHT11 Température, DHT11 Humidité, MQ-2 Fumée",
  "statut": "actif",
  "derniere_connexion": "2024-12-07T01:40:00"
}

D. Mesures (Données des capteurs)
Liste des mesures
httpGET /api/mesures
GET /api/mesures?capteur_id=10
GET /api/mesures?noeud_id=4
GET /api/mesures?limit=50
GET /api/mesures?date_debut=2024-12-07T00:00:00&date_fin=2024-12-07T23:59:59
Authorization: Bearer {token}

Response 200:
[
  {
    "id": 1234,
    "capteur_id": 10,
    "capteur_nom": "DHT11 Température",
    "type": "temperature",
    "unite": "°C",
    "noeud_id": 4,
    "noeud_nom": "NodeMCU-ESP32-Test",
    "localisation": ".........",
    "valeur": 23.5,
    "timestamp": "2024-12-07T01:40:45",
    "metadata": null
  }
]
Dernière mesure d'un capteur
httpGET /api/mesures/derniere/{capteur_id}
GET /api/mesures/derniere/{capteur_id}?noeud_id=4
Authorization: Bearer {token}

Response 200:
{
  "id": 1234,
  "capteur_nom": "DHT11 Température",
  "type": "temperature",
  "unite": "°C",
  "noeud_nom": "NodeMCU-ESP32-Test",
  "valeur": 23.5,
  "timestamp": "2024-12-07T01:40:45"
}

Statistiques des mesures
httpGET /api/mesures/statistiques
GET /api/mesures/statistiques?capteur_id=10
GET /api/mesures/statistiques?noeud_id=4
GET /api/mesures/statistiques?date_debut=2024-12-07T00:00:00
Authorization: Bearer {token}

Response 200:
[
  {
    "capteur_nom": "DHT11 Température",
    "type": "temperature",
    "unite": "°C",
    "total_mesures": 1458,
    "moyenne": 23.45,
    "minimum": 18.2,
    "maximum": 28.9,
    "ecart_type": 2.15,
    "premiere_mesure": "2024-12-01T10:00:00",
    "derniere_mesure": "2024-12-07T01:40:45"
  }
]
Historique avec agrégation
httpGET /api/mesures/historique?capteur_id=10&intervalle=heure&limit=24
GET /api/mesures/historique?capteur_id=10&intervalle=jour&limit=7
Authorization: Bearer {token}

Paramètres:
- intervalle: "heure" | "jour" | "semaine"
- limit: nombre de périodes

Response 200:
[
  {
    "periode": "2024-12-07 00:00:00",
    "moyenne": 23.5,
    "minimum": 22.1,
    "maximum": 25.3,
    "nombre_mesures": 360
  },
  {
    "periode": "2024-12-07 01:00:00",
    "moyenne": 23.8,
    "minimum": 23.2,
    "maximum": 24.5,
    "nombre_mesures": 240
  }
]

E. Alertes
Liste des alertes
httpGET /api/alertes
GET /api/alertes?actif=true
GET /api/alertes?severite=critical
Authorization: Bearer {token}

Response 200:
[
  {
    "id": 10,
    "capteur_id": 10,
    "capteur_nom": "DHT11 Température",
    "type": "temperature",
    "noeud_id": 4,
    "noeud_nom": "NodeMCU-ESP32-Test",
    "type_alerte": "seuil_max",
    "severite": "warning",
    "seuil_min": null,
    "seuil_max": 35.0,
    "message": "Température élevée",
    "email_notification": true,
    "actif": true,
    "date_creation": "2024-12-01T10:00:00"
  }
]
Historique des alertes déclenchées
httpGET /api/alertes/logs
GET /api/alertes/logs?alerte_id=10
GET /api/alertes/logs?limit=50
Authorization: Bearer {token}

Response 200:
[
  {
    "id": 567,
    "alerte_id": 10,
    "type_alerte": "anomalie",
    "severite": "critical",
    "alerte_message": "Risque d'incendie détecté par l'IA",
    "capteur_nom": "DHT11 Température",
    "noeud_nom": "NodeMCU-ESP32-Test",
    "mesure_id": 1234,
    "valeur_mesuree": 85.5,
    "message": "IA: CRITICAL - Risque: 85.5% | T=45°C, H=25%, Fumée=450ppm",
    "email_envoye": true,
    "date_email": "2024-12-07T01:41:00",
    "timestamp": "2024-12-07T01:40:50"
  }
]
Créer une alerte
httpPOST /api/alertes
Authorization: Bearer {token}
Content-Type: application/json

Body:
{
  "capteur_id": 10,
  "noeud_id": 4,
  "type_alerte": "seuil_max",
  "severite": "warning",
  "seuil_max": 35.0,
  "message": "Température trop élevée",
  "email_notification": true
}

Response 201:
{
  "message": "Alerte créée",
  "id": 15
}


G. IA (Prédictions)
Prédiction manuelle
httpPOST /api/ia/predict
Authorization: Bearer {token}
Content-Type: application/json

Body:
{
  "temperature": 45.0,
  "humidity": 25.0,
  "smoke_level": 450.0
}

Response 200:
{
  "prediction": 1,
  "fire_risk_percent": 85.5,
  "status": "CRITICAL",
  "confidence": 92.3,
  "smoke_level": 450.0
}

# Status possibles: "SAFE", "WARNING", "CRITICAL"
Statut du modèle IA
httpGET /api/ia/status
Authorization: Bearer {token}

Response 200:
{
  "model_loaded": true,
  "model_path": "models/fire_model.pkl"
}

H. Logs système
httpGET /api/logs
GET /api/logs?niveau=error
GET /api/logs?noeud_id=4
GET /api/logs?limit=100
Authorization: Bearer {token}

# Réservé aux admins
