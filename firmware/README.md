# 🌱 Smart Garden System - Firmware

> ESP32-S3 microcontroller firmware for autonomous garden management

This folder contains the source code for the Maker Feather AIoT S3 (ESP32-S3) microcontroller. The firmware manages real-time sensor data acquisition, autonomous irrigation logic, and bi-directional communication with Google Firebase.

---

## 🚀 Features

- **Hybrid Control**: Supports both AUTO (sensor-based) and MANUAL (dashboard-controlled) modes
- **Smart Irrigation**: Monitors soil moisture and rain levels to prevent over-watering
- **Automated Fertilization**: Triggered automatically alongside the water pump at 24-hour intervals
- **Real-time Logging**: Pushes sensor data and historical logs to Firebase RTDB every 10 seconds
- **Visual Feedback**: 3-stage LED status indicators (Red/Yellow/Green)

---

## 🛠 Hardware Configuration

### Pin Mapping

| Component | Pin / Label | Code Variable | Function |
|-----------|-------------|---------------|----------|
| Moisture Sensor | A0 | `moisturePin` | Analog Soil Moisture Reading |
| Rain Sensor | A1 | `rainPin` | Digital/Analog Rain Detection |
| DHT11 Sensor | A2 (D6) | `DHTPIN` | Temp & Humidity Data |
| Water Pump | A4 | `waterPumpPin` | Relay Control (Pump 1) |
| Fertilizer Pump | A5 | `fertilizerPumpPin` | Relay Control (Pump 2) |
| Green LED | D5 | `ledGreen` | Status: System Normal |
| Yellow LED | D7 | `ledYellow` | Status: Warning / Manual Mode |
| Red LED | D9 | `ledRed` | Status: Critical / Pump Active |

### Calibration Notes

**Soil Moisture:**
- 0% (Dry) is set at a raw analog value of **500**
- 100% (Wet) is set at **0**

**Relay Logic:**  
The code is configured for Active-Low relays:
- `digitalWrite(pin, LOW)` = Relay ON (Pump Active)
- `digitalWrite(pin, HIGH)` = Relay OFF (Pump Idle)

---

## 💻 Installation & Setup

### Prerequisites

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Add ESP32 Board Support:
   - Go to **File > Preferences > Additional Boards Manager URLs**
   - Add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`

### Required Libraries

Install the following via the Arduino Library Manager:

- **Firebase Arduino Client Library for ESP8266 and ESP32** by Mobizt
- **DHT sensor library** by Adafruit
- **Adafruit Unified Sensor**

### Configuration

Update the following constants in `SmartGarden.ino` with your credentials:

```cpp
#define WIFI_SSID "Your_WiFi_Name"
#define WIFI_PASSWORD "Your_WiFi_Password"
#define API_KEY "Your_Firebase_API_Key"
#define DATABASE_URL "Your_Firebase_Database_URL"
```

### Upload

1. Select Board: **Maker Feather AIoT S3** (or ESP32S3 Dev Module)
2. Click **Upload**

---

## 📁 Code Structure

### Main Functions

**`setup()`**  
Initializes Serial communication, GPIO pins, Wi-Fi connection, and Firebase authentication.

**`loop()`**  
Core execution loop that:
- Reads sensor values
- Listens for `/garden/controls/mode` from Firebase
- Executes irrigation logic based on moisture thresholds
- Pushes sensor data to `/garden/history` at 10-second intervals

**`printStatus()`**  
Local debugging output via Serial Monitor (115200 baud)

---

## 📊 Firebase Data Structure

### Control Path
```
/garden/controls/mode → "AUTO" | "MANUAL"
```

### History Path
```
/garden/history/{timestamp}
├── moisture: number
├── temperature: number
├── humidity: number
├── rain: boolean
└── timestamp: string
```

---

## 🔧 Troubleshooting

**Wi-Fi Connection Issues:**
- Verify SSID and password
- Check 2.4GHz network compatibility (ESP32 doesn't support 5GHz)

**Sensor Reading Errors:**
- Confirm pin connections match configuration
- Check power supply voltage (3.3V for ESP32-S3)

**Firebase Communication:**
- Validate API key and database URL
- Check Firebase security rules
- Monitor Serial output for authentication errors

---

## 📝 License

This project is part of the Smart Garden System. Refer to the main repository for license information.
