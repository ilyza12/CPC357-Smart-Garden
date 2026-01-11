# 🌱 Smart Garden System (IoT)

**Cultivating Nature in Your Own Space.**
A fully automated, cloud-connected IoT solution designed to make urban gardening effortless. This system automates watering and fertilization based on real-time environmental data, helping users maintain healthy plants while conserving water (aligned with **UN SDG 11**).

---

## 🔭 Project Overview
The **Smart Garden System** addresses the challenges of modern urban living—lack of time and gardening knowledge. By integrating an **ESP32 microcontroller** with **Google Firebase** and a **Vue.js Dashboard**, the system monitors vital parameters (Soil Moisture, Rain, Temperature, Humidity) and autonomously manages plant care.

### 🌍 SDG Impact
This project contributes to **Goal 11: Sustainable Cities and Communities** by:
* **Target 11.6:** Reducing water wastage through precision moisture-based irrigation.
* **Target 11.3:** Lowering barriers to entry for urban agriculture.

---

## ✨ Key Features
* **🌱 Automated Care:** Auto-triggers watering when soil moisture drops below threshold (if no rain is detected).
* **🧪 Scheduled Fertilization:** Time-based automated nutrient delivery.
* **📱 Real-Time Dashboard:** Monitor live stats (Temperature, Humidity, Soil Moisture) from anywhere.
* **🎮 Dual Control Modes:**
    * **Auto Mode:** The system decides based on sensor logic.
    * **Manual Mode:** User controls pumps directly via the dashboard.
* **📊 Data Visualization:** Historical charts for environmental trends.

---

## 🏗 System Architecture
The system follows a **3-Layer IoT Architecture**:
1.  **Perception Layer (Hardware):** ESP32, Sensors, and Actuators.
2.  **Network Layer (Cloud):** Google Firebase Realtime Database for instant synchronization.
3.  **Application Layer (Frontend):** Vue.js Web Dashboard for visualization and control.

---

## 💻 Tech Stack
* **Firmware:** C++ / Arduino IDE
* **Frontend:** Vue.js 3, Chart.js
* **Backend/Database:** Google Firebase Realtime Database
* **Communication:** Wi-Fi (REST & WebSockets)

---

## 🚀 Getting Started

### 1. Firmware Setup (ESP32)


### 2. Frontend Setup (Vue.js)
1.  Navigate to the web app folder:
    ```bash
    cd frontend
    ```
2.  Install dependencies:
    ```bash
    npm install
    ```
3.  Run the development server:
    ```bash
    npm run dev
    ```

---

## 👥 Team (Group 33)
* **Shereen Ilyza Binti Sheik Mujibu Rahman** (164718)
* **Sabrina Binti Sofian** (164740)

---

*Project for CPC357: IoT Architecture & Smart Applications @ USM.*
