# 📡 Wireless Sensor Network Routing using ESP32

This project implements a **Wireless Sensor Network (WSN)** using **ESP32 microcontrollers** and **DHT11 sensors**, with real-time data collection, optimized routing, and cloud visualization through the **Blynk IoT Platform**.

---

## 🚀 Features

- 🌡️ Real-time temperature and humidity sensing via DHT11
- 📶 Multi-node communication over Wi-Fi using ESP-NOW
- 🔁 Routing with **Dijkstra’s algorithm** for optimized path selection
- 📲 Real-time data visualization on **Blynk Cloud**
- 📈 Graph-based sensor data view for each node
- ✅ Indoor testing with >88% packet delivery success
- 🔐 MAC-based node identification and basic fault recovery

---

## 🧠 Technologies Used

- **Hardware**:  
  - ESP32 Dev Boards  
  - DHT11 Sensors
  - Data Processing Cables  

- **Software**:  
  - Arduino IDE (ESP32 core)  
  - C/C++ Embedded Programming  
  - Blynk IoT Platform (Mobile App + Cloud)  
  - Dijkstra’s Algorithm (Custom Implementation)

---

## 📲 Blynk Dashboard Overview

- View temperature/humidity values from each node in real-time
- Monitor routing table dynamically
- Visual graph output for each node’s data stream

---

## 📷 Screenshots

### 🔸 Mobile Dashboard UI
(![WhatsApp Image 2025-07-30 at 6 07 16 PM](https://github.com/user-attachments/assets/5d195de3-dd8c-4c24-8183-d466322d1208))  
> Real-time sensor readings and routing table from 3 ESP32 nodes

### 🔸 Hardware Setup
(![WhatsApp Image 2025-07-30 at 6 07 15 PM](https://github.com/user-attachments/assets/92bebf12-4d0c-44ca-83c5-7753102a9fde))  
> ESP32 boards connected with sensors and programmed via laptops

### 🔸 Code & Serial Monitor
(![WhatsApp Image 2025-07-30 at 6 07 14 PM](https://github.com/user-attachments/assets/c721230c-69b9-4924-bc94-ef638eaa92d4))  
> Routing data logs and temperature readings sent from nodes

---

## 📁 How to Run the Project

1. Install the required libraries in Arduino IDE:  
   - `BlynkSimpleEsp32.h`  
   - `esp_now.h`  
   - `WiFi.h`

2. Flash the code to each ESP32 node with:
   - Unique MAC address per node
   - Same Wi-Fi SSID & password
   - Individual Blynk Auth Token

3. Use the Blynk mobile app:
   - Add 3 value displays for Node 1, 2, and 3
   - Add graph widgets for each node
   - Add terminal widget to monitor logs

4. Power all ESP32 nodes and observe routing + sensor data in real-time

---

## 📊 Sample Output (Serial Monitor)

