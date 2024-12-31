# Plant Care System

## Overview
The **Plant Care System** is a smart monitoring and automation system designed to help users maintain healthy plants. It uses an ESP8266 microcontroller to collect data from various sensors (soil moisture, temperature, humidity, and light intensity) and provides real-time feedback and recommendations through a web interface. The system also includes an automated water pump to ensure plants receive adequate hydration.

---

## Features
1. **Real-Time Sensor Monitoring**:
   - Soil moisture level.
   - Ambient temperature.
   - Humidity level.
   - Light intensity.

2. **Automated Watering**:
   - Activates a water pump when soil moisture is low.
   - Includes safety features to prevent overwatering.

3. **Web Interface**:
   - Displays real-time sensor data.
   - Provides plant care recommendations.
   - Visualizes potential risks and solutions.

4. **Smart Analysis**:
   - Uses AI (via Gemini API) to analyze sensor data and provide actionable insights.

5. **Safety Features**:
   - Emergency stop function.
   - Watchdog timer to prevent system hangs.
   - Sensor health checks.

---

## Hardware Requirements
- **ESP8266 Microcontroller** (e.g., NodeMCU, Wemos D1 Mini).
- **Soil Moisture Sensor** (analog or digital).
- **DHT11 Sensor** (for temperature and humidity).
- **LDR Sensor** (for light intensity).
- **Relay Module** (to control the water pump).
- **Water Pump** (submersible or external).
- **Jumper Wires**.
- **Breadboard** (optional).

---

## Pin Connections
| Component               | ESP8266 Pin |
|-------------------------|-------------|
| Soil Moisture Sensor    | A0          |
| DHT11 Sensor            | D4          |
| LDR Sensor              | D2          |
| Relay Module            | D1          |

---

## Software Requirements
1. **Arduino IDE** (with ESP8266 core installed).
2. **Libraries**:
   - **ESP8266WiFi**: For WiFi connectivity.
   - **ESP8266WebServer**: For hosting the web interface.
   - **DHT**: For reading temperature and humidity data.
   - **ArduinoJson**: For JSON data handling.

---

## Setup Instructions
1. **Hardware Setup**:
   - Connect the sensors and relay module to the ESP8266 as per the pin connections table.
   - Connect the water pump to the relay module.

2. **Software Setup**:
   - Install the required libraries in the Arduino IDE.
   - Open the provided code in the Arduino IDE.
   - Update the WiFi credentials (`ssid` and `password`) in the code.
   - Upload the code to the ESP8266.

3. **Web Interface**:
   - After uploading the code, open the Serial Monitor to view the ESP8266's IP address.
   - Open a web browser and navigate to the IP address to access the web interface.

4. **Calibration** (Optional):
   - Use the `calibrateSoilMoistureSensor()` function to calibrate the soil moisture sensor for accurate readings.

---

## Web Interface Features
- **Real-Time Metrics**:
  - Displays soil moisture, humidity, temperature, light intensity, and pump status.
- **Plant Care Analysis**:
  - Provides recommendations based on sensor data.
  - Highlights potential risks (e.g., dehydration, root rot).
- **AI-Powered Insights**:
  - Uses the Gemini API to generate advanced plant care suggestions.

---

## Code Structure
- **`setup()`**: Initializes sensors, WiFi, and the web server.
- **`loop()`**: Handles client requests, updates sensor readings, and controls the pump.
- **`handleRoot()`**: Serves the HTML web interface.
- **`handleSensorData()`**: Returns sensor data in JSON format.
- **`updateSensorReadings()`**: Reads data from all sensors.
- **`controlPump()`**: Activates the pump based on soil moisture levels.
- **`checkSensorHealth()`**: Monitors sensor health and disables the pump if errors are detected.
- **`emergencyStop()`**: Stops the pump and halts the system in case of an emergency.

---

## Safety Features
1. **Pump Timeout**:
   - The pump automatically turns off after 10 seconds to prevent overwatering.
2. **High Moisture Protection**:
   - The pump is disabled if soil moisture exceeds 80%.
3. **Emergency Stop**:
   - Stops the pump and halts the system until manually reset.
4. **Watchdog Timer**:
   - Prevents system hangs by resetting the ESP8266 if it becomes unresponsive.

---

## Troubleshooting
1. **No WiFi Connection**:
   - Ensure the correct SSID and password are entered.
   - Check the WiFi signal strength.
2. **Incorrect Sensor Readings**:
   - Verify sensor connections.
   - Calibrate the soil moisture sensor if necessary.
3. **Pump Not Working**:
   - Check the relay module and pump connections.
   - Ensure the pump is receiving power.

Enjoy your **Plant Care System** and keep your plants healthy and happy! 🌱😊
