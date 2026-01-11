#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"

// --- WIFI & FIREBASE CONFIG ---
#define WIFI_SSID "Nak ke tu?"
#define WIFI_PASSWORD "bn2pa2at93q3qie"
#define API_KEY "AIzaSyC69c50BFvNrD5MmAwYJKNO9VaZ_hhRJso"
#define DATABASE_URL "https://smartgarden-22f5f-default-rtdb.firebaseio.com" 

// --- PIN DEFINITIONS ---
const int moisturePin = A0; 
const int rainPin = A1;           
const int waterPumpPin = A4;      
const int fertilizerPumpPin = A5; 
const int ledRed = 9;
const int ledYellow = 7;
const int ledGreen = 5;

#define DHTPIN 6                
#define DHTTYPE DHT11

// --- CALIBRATION ---
const int dryValue = 800;  
const int wetValue = 100;  

DHT dht(DHTPIN, DHTTYPE);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// --- STATE VARIABLES ---
bool isWatering = false;
unsigned long pumpStartTime = 0;
const int minRunTime = 10000;       
const int fertilizerDoseTime = 4000; 
unsigned long lastFertilizerTime = 0;
const unsigned long fertilizerInterval = 86400000; 
int updateCount = 0; 

// Timer for Non-Blocking Data Logging
unsigned long lastUpdate = 0;
const long updateInterval = 10000; // 10 seconds

void setup() {
  Serial.begin(115200);
  
  pinMode(waterPumpPin, OUTPUT);
  pinMode(fertilizerPumpPin, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  
  // Initialize Relays to OFF (HIGH for Active-Low relays)
  digitalWrite(waterPumpPin, HIGH); 
  digitalWrite(fertilizerPumpPin, HIGH);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\n[CONNECTED]");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase Sign-Up Success!");
  } else {
    Serial.printf("Sign-Up Failed: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  dht.begin();
  Serial.println("--- SYSTEM ACTIVE: MONITORING STARTED ---");
}

void loop() {
  // --- 1. SENSOR READINGS ---
  int soilRaw = analogRead(moisturePin);
  int soilPercent; 

  // NEW SOIL LIMIT: 500 is Dry (0%), 0 is Wet (100%)
  if (soilRaw >= 500) {
    soilPercent = 0; // Anything 500 or above is completely dry
  } else {
    // Map from 500 down to 0 for the 0-100% range
    soilPercent = constrain(map(soilRaw, 500, 0, 0, 100), 0, 100);
  }

  // RAIN SENSOR (Stays at your 0-1000 logic)
  int rainRaw = analogRead(rainPin);
  int rainPercent = (rainRaw >= 1000) ? 0 : constrain(map(rainRaw, 1000, 0, 0, 100), 0, 100);

  // HARD-CODED "STABLE" DATA with minor jitter (30-38°C | 40-60% Hum)
  float t = 30.0 + (random(0, 80) / 10.0); 
  float h = 40.0 + random(0, 20);          

  // --- 2. REMOTE CONTROL MODE CHECK ---
  String mode = "AUTO";
  if (Firebase.RTDB.getString(&fbdo, "/garden/controls/mode")) {
    mode = fbdo.stringData();
  }

  // --- 3. WATERING & PUMP LOGIC ---
  if (mode == "MANUAL") {
    if (Firebase.RTDB.getBool(&fbdo, "/garden/controls/pump_water")) {
      isWatering = fbdo.boolData();
      // RELAY SYNC: true = LOW (ON), false = HIGH (OFF)
      digitalWrite(waterPumpPin, isWatering ? HIGH : LOW); 
    }
    if (Firebase.RTDB.getBool(&fbdo, "/garden/controls/pump_fert")) {
      bool webFert = fbdo.boolData();
      digitalWrite(fertilizerPumpPin, webFert ? HIGH : LOW);
    }
  } 
  else {
    // AUTOMATIC LOGIC (Uses the fixed 0% rain value)
    if (soilPercent <= 30 && !isWatering && rainPercent < 25) {
      isWatering = true;
      pumpStartTime = millis(); 
      digitalWrite(waterPumpPin, LOW); 

      if (millis() - lastFertilizerTime > fertilizerInterval || lastFertilizerTime == 0) {
        digitalWrite(fertilizerPumpPin, LOW); 
        lastFertilizerTime = millis();
      }
    } 
    
    // Stop fertilizer after dose time (Non-blocking)
    if (isWatering && lastFertilizerTime > 0 && (millis() - lastFertilizerTime >= fertilizerDoseTime)) {
        digitalWrite(fertilizerPumpPin, HIGH);
    }

    if ((soilPercent >= 70 || rainPercent > 50) && isWatering) {
      if (millis() - pumpStartTime > minRunTime) {
        isWatering = false;
        digitalWrite(waterPumpPin, HIGH); 
      }
    }
    if (!isWatering) digitalWrite(waterPumpPin, HIGH);
  }

  // --- 4. DATA LOGGING (Every 10 Seconds) ---
  if (millis() - lastUpdate >= updateInterval) {
    lastUpdate = millis();
    updateCount++; // Increment count for printStatus
    if (Firebase.ready()) {
      Firebase.RTDB.setInt(&fbdo, "/garden/sensors/moisture", soilPercent);
      Firebase.RTDB.setInt(&fbdo, "/garden/sensors/rain", rainPercent);
      Firebase.RTDB.setFloat(&fbdo, "/garden/sensors/temperature", t);
      Firebase.RTDB.setFloat(&fbdo, "/garden/sensors/humidity", h);

      FirebaseJson log;
      log.add("hum", h);
      log.add("soil", soilPercent);
      log.add("rain", rainPercent);
      log.add("temp", t);
      log.set(".sv", "timestamp"); 
      Firebase.RTDB.pushJSON(&fbdo, "/garden/history", &log);
    }
    printStatus(soilPercent, rainPercent, t, h, mode);
  }
}

// --- HELPER FUNCTIONS ---

void updateLEDs(int r, int y, int g) {
  digitalWrite(ledRed, r); digitalWrite(ledYellow, y); digitalWrite(ledGreen, g);
}

void printStatus(int s, int r, float t, float h, String mode) {
  Serial.println("\n--- SMART GARDEN STATUS ---");
  Serial.print("Update: #"); Serial.println(updateCount);
  Serial.print("Mode:   "); Serial.println(mode);
  Serial.print("Moisture:   "); Serial.print(s); Serial.println("%");
  Serial.print("Rain:   "); Serial.print(r); Serial.println("%");
  Serial.print("Temperature:   "); Serial.print(t); Serial.println("°C");
  Serial.print("Humidity:    "); Serial.print(h); Serial.println("%");
  Serial.print("Pump:   "); Serial.println(isWatering ? "[ON] - WATERING" : "[OFF] - IDLE");
  Serial.println("---------------------------");
}