#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Pin Definitions
#define SOIL_MOISTURE_PIN A0
#define DHT_PIN D4
#define LDR_PIN D2
#define PUMP_PIN D1
#define DHT_TYPE DHT11

// Network credentials
const char* ssid = "Kossn-Lab-2";
const char* password = "L12345678";

// Initialize sensors
DHT dht(DHT_PIN, DHT_TYPE);
ESP8266WebServer server(80);

// HTML content (previously shown web interface)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Plant Care System</title>
    <link href="https://cdnjs.cloudflare.com/ajax/libs/tailwindcss/2.2.19/tailwind.min.css" rel="stylesheet">
    <link href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css" rel="stylesheet">
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap');
        body {
            font-family: 'Inter', sans-serif;
            background: linear-gradient(135deg, #f6f9fc 0%, #eef2f7 100%);
            min-height: 100vh;
            padding: 2rem;
        }
        .sensor-value {
            background: linear-gradient(135deg, #f8fafc 0%, #f1f5f9 100%);
            transition: all 0.3s ease;
            padding: 1rem;
            border-radius: 0.5rem;
            display: flex;
            align-items: center;
        }
        .sensor-value:hover {
            transform: translateY(-2px);
        }
        .recommendation-card {
            border-left: 4px solid;
            transition: all 0.3s ease;
            padding: 1rem;
            margin-bottom: 0.5rem;
            border-radius: 0.5rem;
        }
        .recommendation-card:hover {
            transform: translateX(4px);
        }
        .loading-spinner {
            border: 3px solid #f3f3f3;
            border-top: 3px solid #3498db;
            border-radius: 50%;
            width: 24px;
            height: 24px;
            animation: spin 1s linear infinite;
        }
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        .classic-image {
            width: 200px;
            height: auto;
        }
        .arrow-point {
            list-style-type: none;
            padding-left: 1rem;
        }
        .arrow-point::before {
            content: '→';
            margin-right: 0.5rem;
            color: #3498db;
        }
        .bold-heading {
            font-weight: bold;
        }
        @media (max-width: 768px) {
            .grid-cols-1 {
                grid-template-columns: 1fr;
            }
            .grid-cols-3 {
                grid-template-columns: 1fr;
            }
            .classic-image {
                width: 100%;
            }
        }
        @media (min-width: 769px) and (max-width: 1024px) {
            .grid-cols-1 {
                grid-template-columns: 1fr;
            }
            .grid-cols-3 {
                grid-template-columns: 1fr 1fr;
            }
            .classic-image {
                width: 100%;
            }
        }
    </style>
</head>
<body>
    <!-- Header -->
    <header class="text-center mb-8">
        <h1 class="text-4xl md:text-5xl font-bold bg-gradient-to-r from-green-600 to-blue-600 bg-clip-text text-transparent">Plant Care System</h1>
        <p class="text-gray-600 mt-2">Smart monitoring for healthy plants</p>
    </header>

    <!-- Main Content -->
    <div class="grid grid-cols-1 lg:grid-cols-2 gap-8">
        <!-- Sensor Data -->
        <div>
            <h2 class="text-2xl font-semibold mb-6">Real-time Metrics</h2>
            <div class="grid grid-cols-1 md:grid-cols-3 gap-4">
                <div class="sensor-value">
                    <i class="fas fa-tint text-blue-500 text-2xl mr-2"></i>
                    <div>
                        <h3 class="text-gray-600">Soil Moisture</h3>
                        <div class="text-3xl font-bold text-gray-800" id="soil-moisture">--</div>
                    </div>
                </div>
                <div class="sensor-value">
                    <i class="fas fa-tint text-blue-500 text-2xl mr-2"></i>
                    <div>
                        <h3 class="text-gray-600">Humidity</h3>
                        <div class="text-3xl font-bold text-gray-800" id="humidity">--</div>
                    </div>
                </div>
                <div class="sensor-value">
                    <i class="fas fa-thermometer-half text-red-500 text-2xl mr-2"></i>
                    <div>
                        <h3 class="text-gray-600">Temperature</h3>
                        <div class="text-3xl font-bold text-gray-800" id="temperature">--</div>
                    </div>
                </div>
                <div class="sensor-value">
                    <i class="fas fa-sun text-yellow-500 text-2xl mr-2"></i>
                    <div>
                        <h3 class="text-gray-600">Light Intensity</h3>
                        <div class="text-3xl font-bold text-gray-800" id="light-intensity">--</div>
                    </div>
                </div>
                <div class="sensor-value">
                    <i class="fas fa-water text-blue-500 text-2xl mr-2"></i>
                    <div>
                        <h3 class="text-gray-600">Water Pump</h3>
                        <div class="text-3xl font-bold text-gray-800" id="water-pump-status">--</div>
                    </div>
                </div>
            </div>
        </div>

        <!-- Image Section -->
        <div class="flex items-center justify-center">
            <img src="https://i.imgur.com/IjHcicX.jpg" alt="Plant Image" class="classic-image">
        </div>
    </div>

    <!-- Analysis Section -->
    <div class="mt-8">
        <h2 class="text-2xl font-semibold">Plant Care Analysis</h2>
        <div class="text-sm text-gray-500" id="timestamp"></div>
        <div id="analysis-content" class="space-y-4">
            <!-- Analysis content will be inserted here -->
        </div>
    </div>

    <!-- Loading Indicator -->
    <div id="loading-indicator" class="hidden">
        <div class="loading-spinner mx-auto"></div>
    </div>

    <script>
        const soilMoistureElement = document.querySelector("#soil-moisture");
        const humidityElement = document.querySelector("#humidity");
        const temperatureElement = document.querySelector("#temperature");
        const lightIntensityElement = document.querySelector("#light-intensity");
        const waterPumpStatusElement = document.querySelector("#water-pump-status");
        const suggestionsList = document.querySelector("#analysis-content");
        const loadingIndicator = document.querySelector("#loading-indicator");

        // API configuration
        const API_KEY = "AIzaSyBvAeFWsghSnLjW2xJyXefYHtn-Per5Cyo";
        const API_URL = `https://generativelanguage.googleapis.com/v1/models/gemini-1.5-pro:generateContent?key=${API_KEY}`;

        let sensorData = {};

        const updateSensorData = async () => {
            try {
                const response = await fetch('/sensor-data');
                sensorData = await response.json();

                soilMoistureElement.textContent = `${sensorData.soilMoisture}%`;
                humidityElement.textContent = `${sensorData.humidity}%`;
                temperatureElement.textContent = `${sensorData.temperature}°C`;
                lightIntensityElement.textContent = `${sensorData.lightIntensity}`;
                waterPumpStatusElement.textContent = sensorData.pumpStatus ? 'ON' : 'OFF';

                const analysisHtml = generateAnalysis(sensorData);
                suggestionsList.innerHTML = analysisHtml;
                document.getElementById('timestamp').textContent = `Last updated: ${new Date().toLocaleTimeString()}`;
                generateAPIResponse();
            } catch (error) {
                console.error('Error fetching sensor data:', error);
            }
        };

        const generateAnalysis = (sensorData) => {
            let recommendations = [];
            let risks = [];

            if (sensorData.soilMoisture < 30) {
                recommendations.push('Increase watering frequency');
                risks.push('Risk of plant dehydration');
            } else if (sensorData.soilMoisture > 70) {
                recommendations.push('Reduce watering');
                risks.push('Risk of root rot');
            }

            if (sensorData.humidity < 40) {
                recommendations.push('Use a humidity tray or mister');
                risks.push('Low humidity may cause leaf browning');
            } else if (sensorData.humidity > 80) {
                recommendations.push('Improve air circulation');
                risks.push('High humidity increases risk of fungal growth');
            }

            if (sensorData.temperature < 15) {
                recommendations.push('Move plant to a warmer location');
                risks.push('Low temperature may slow growth');
            } else if (sensorData.temperature > 30) {
                recommendations.push('Provide shade or move to a cooler location');
                risks.push('High temperature may cause stress');
            }

            if (sensorData.lightIntensity == 0) {
                recommendations.push('Increase light exposure');
                risks.push('Low light may cause slow growth');
            } else if (sensorData.lightIntensity == 1) {
                recommendations.push('Reduce light exposure');
                risks.push('High light may cause leaf burn');
            }

            return `
                <div class="space-y-6">
                    ${recommendations.length > 0 ? `
                        <h3 class="text-lg font-semibold text-gray-800 bold-heading">Recommendations</h3>
                        <ul class="list-none space-y-2">
                            ${recommendations.map(rec => `<li class="arrow-point">${rec}</li>`).join('')}
                        </ul>` : ''}
                    ${risks.length > 0 ? `
                        <h3 class="text-lg font-semibold text-red-800 mb-2 bold-heading">Potential Risks</h3>
                        <ul class="list-none space-y-2">
                            ${risks.map(risk => `<li class="arrow-point text-red-700">${risk}</li>`).join('')}
                        </ul>` : ''}
                </div>`;
        };

        const generateAPIResponse = async () => {
            try {
                loadingIndicator.style.display = 'block';
                suggestionsList.innerHTML = '<div class="text-center text-gray-500">Analyzing plant data...</div>';
                const response = await fetch(API_URL, {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify({
                        contents: [{
                            role: "user",
                            parts: [{ text: `Soil Moisture: ${sensorData.soilMoisture}%, Humidity: ${sensorData.humidity}%, Temperature: ${sensorData.temperature}°C, Light Intensity: ${sensorData.lightIntensity}` }]
                        }]
                    }),
                });
                const data = await response.json();
                if (!response.ok) throw new Error(data.error.message);

                const apiResponse = data.candidates[0].content.parts[0].text.replace(/\*\*(.*?)\*\*/g, '$1');
                const apiRecommendations = apiResponse.split('\n').filter(line => line.trim() !== '');

                suggestionsList.innerHTML += `
                    <ul class='list-none space-y-2'>
                        ${apiRecommendations.map(rec => `<li class="arrow-point">${rec}</li>`).join('')}
                    </ul>`;
            } catch (error) {
                suggestionsList.innerHTML += `
                    <p class='text-red-700'>Error: ${error.message}</p>`;
            } finally {
                loadingIndicator.style.display = 'none';
            }
        };

        // Initial update and set interval
        updateSensorData();
        setInterval(updateSensorData, 30000); // Update every 30 seconds
    </script>
</body>
</html>
)rawliteral";

// Sensor data structure
struct SensorData {
    float soilMoisture;
    float humidity;
    float temperature;
    int lightIntensity;
    bool pumpStatus;
};

SensorData currentSensorData;

// Forward declarations
void handleRoot();
void handleSensorData();
void updateSensorReadings();
void controlPump();

void setup() {
    Serial.begin(115200);

    // Initialize pins
    pinMode(SOIL_MOISTURE_PIN, INPUT);
    pinMode(LDR_PIN, INPUT);
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW);

    // Initialize DHT sensor
    dht.begin();

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Setup server routes
    server.on("/", HTTP_GET, handleRoot);
    server.on("/sensor-data", HTTP_GET, handleSensorData);

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
    updateSensorReadings();
    controlPump();
    delay(1000);
}

void handleRoot() {
    server.send(200, "text/html", index_html);
}

void handleSensorData() {
    StaticJsonDocument<200> doc;

    doc["soilMoisture"] = currentSensorData.soilMoisture;
    doc["humidity"] = currentSensorData.humidity;
    doc["temperature"] = currentSensorData.temperature;
    doc["lightIntensity"] = currentSensorData.lightIntensity;
    doc["pumpStatus"] = currentSensorData.pumpStatus;

    String response;
    serializeJson(doc, response);

    server.send(200, "application/json", response);
}

void updateSensorReadings() {
    // Read soil moisture (map raw values to percentage)
    int soilRaw = analogRead(SOIL_MOISTURE_PIN);
    currentSensorData.soilMoisture = map(soilRaw, 1023, 0, 0, 100);

    // Read DHT11 sensor
    currentSensorData.humidity = dht.readHumidity();
    currentSensorData.temperature = dht.readTemperature();

    // Read light intensity (digital value)
    currentSensorData.lightIntensity = digitalRead(LDR_PIN);

    // Check for failed readings
    if (isnan(currentSensorData.humidity) || isnan(currentSensorData.temperature)) {
        Serial.println("Failed to read from DHT sensor!");
    }
}

void controlPump() {
    // Activate pump if soil moisture is too low
    if (currentSensorData.soilMoisture < 30) {
        digitalWrite(PUMP_PIN, HIGH);
        currentSensorData.pumpStatus = true;
    } else {
        digitalWrite(PUMP_PIN, LOW);
        currentSensorData.pumpStatus = false;
    }

    // Safety check - don't run pump for too long
    static unsigned long pumpStartTime = 0;
    static const unsigned long MAX_PUMP_TIME = 10000; // 10 seconds max pump time

    if (currentSensorData.pumpStatus) {
        if (pumpStartTime == 0) {
            pumpStartTime = millis();
        } else if (millis() - pumpStartTime > MAX_PUMP_TIME) {
            digitalWrite(PUMP_PIN, LOW);
            currentSensorData.pumpStatus = false;
            pumpStartTime = 0;
            Serial.println("Pump timeout - safety shutoff");
        }
    } else {
        pumpStartTime = 0;
    }

    // Additional safety checks
    if (currentSensorData.soilMoisture > 80) { // Prevent overwatering
        digitalWrite(PUMP_PIN, LOW);
        currentSensorData.pumpStatus = false;
        Serial.println("High moisture detected - pump disabled");
    }

    // Log pump status changes
    static bool lastPumpStatus = false;
    if (lastPumpStatus != currentSensorData.pumpStatus) {
        Serial.print("Pump status changed to: ");
        Serial.println(currentSensorData.pumpStatus ? "ON" : "OFF");
        lastPumpStatus = currentSensorData.pumpStatus;
    }
}

// Utility function to check sensor health
void checkSensorHealth() {
    bool sensorsOK = true;
    String errorMessage = "";

    // Check DHT sensor
    if (isnan(currentSensorData.humidity) || isnan(currentSensorData.temperature)) {
        sensorsOK = false;
        errorMessage += "DHT sensor error; ";
    }

    // Check soil moisture sensor
    if (currentSensorData.soilMoisture < 0 || currentSensorData.soilMoisture > 100) {
        sensorsOK = false;
        errorMessage += "Soil moisture sensor error; ";
    }

    // Check light sensor
    if (currentSensorData.lightIntensity < 0) {
        sensorsOK = false;
        errorMessage += "Light sensor error; ";
    }

    if (!sensorsOK) {
        Serial.println("Sensor Check Failed: " + errorMessage);
        // Disable pump as precaution if sensors are faulty
        digitalWrite(PUMP_PIN, LOW);
        currentSensorData.pumpStatus = false;
    }
}

// Function to calibrate soil moisture sensor
void calibrateSoilMoistureSensor() {
    Serial.println("Starting soil moisture sensor calibration...");

    // Read dry value
    Serial.println("Please ensure sensor is completely dry");
    Serial.println("Waiting 10 seconds...");
    delay(10000);

    int dryValue = analogRead(SOIL_MOISTURE_PIN);
    Serial.print("Dry Value: ");
    Serial.println(dryValue);

    // Read wet value
    Serial.println("Please submerge sensor in water");
    Serial.println("Waiting 10 seconds...");
    delay(10000);

    int wetValue = analogRead(SOIL_MOISTURE_PIN);
    Serial.print("Wet Value: ");
    Serial.println(wetValue);

    // Store calibration values in EEPROM if needed
    // EEPROM.write(0, dryValue);
    // EEPROM.write(1, wetValue);

    Serial.println("Calibration complete!");
}

// Emergency stop function
void emergencyStop() {
    digitalWrite(PUMP_PIN, LOW);
    currentSensorData.pumpStatus = false;
    Serial.println("EMERGENCY STOP ACTIVATED");

    // Optionally, could add a red LED indicator here
    // digitalWrite(EMERGENCY_LED_PIN, HIGH);

    while(true) {
        // Stay in emergency stop until reset
        Serial.println("System in emergency stop. Reset required.");
        delay(5000);
    }
}

// Optional: Add watchdog timer to prevent system hangs
void setupWatchdog() {
    ESP.wdtDisable();
    ESP.wdtEnable(WDTO_8S);  // 8 second watchdog timer
}

void keepWatchdogAlive() {
    ESP.wdtFeed();
}

// Optional: Add WiFi reconnection logic
void checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost. Reconnecting...");
        WiFi.begin(ssid, password);

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nWiFi reconnected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("\nWiFi reconnection failed");
        }
    }
}
