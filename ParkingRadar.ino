#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

// --- HARDWARE PINS ---
#define TRIG_PIN 5
#define ECHO_PIN 18
#define IR_FRONT_PIN 4
#define BUZZER_PIN 13
#define RED_LED 19
#define GREEN_LED 21

// --- WI-FI SETTINGS ---
const char* ssid = "xxxxxxx";
const char* password = "xxxxxxx";

WebServer server(80);

int currentDistanceCm = 999;
bool isFrontBlocked = false;

// --- TIMER VARIABLES FOR NON-BLOCKING LOGIC ---
unsigned long previousSensorMillis = 0; 
unsigned long previousBuzzerMillis = 0;
bool buzzerIsOn = false; 
const long sensorInterval = 100; // Check sensors every 100ms

// --- SEND SENSOR DATA TO THE UI ---
void handleData() {
  String json = "{";
  json += "\"frontBlocked\":" + String(isFrontBlocked ? "true" : "false") + ",";
  json += "\"rearDistance\":" + String(currentDistanceCm);
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG_PIN, OUTPUT); 
  pinMode(ECHO_PIN, INPUT); 
  pinMode(IR_FRONT_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT); 
  pinMode(RED_LED, OUTPUT); 
  pinMode(GREEN_LED, OUTPUT);
  
  // 1. SAFE LITTLEFS MOUNT
  if(!LittleFS.begin(true)){
    Serial.println("Error mounting LittleFS! Did you upload the data folder?");
    // Stop the ESP32 safely here so it doesn't crash the IDE!
    while(true) { delay(1000); } 
  }
  
  // 2. CONNECT TO WI-FI (Safe Version)
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  int wifiAttempts = 0;
  // Try for 10 seconds (20 attempts * 500ms)
  while (WiFi.status() != WL_CONNECTED && wifiAttempts < 20) { 
    delay(500); 
    Serial.println("."); // Safe printing
    wifiAttempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n--- CONNECTED ---");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n--- WI-FI FAILED TO CONNECT ---");
    Serial.println("Check your SSID and Password at the top of the code!");
  }

  // 3. START THE WEB SERVER (Explicit Routing)
  
  // Explicitly serve index.html when the root IP is requested
  server.on("/", []() {
    File file = LittleFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  });

  // Explicitly serve the CSS file
  server.on("/style.css", []() {
    File file = LittleFS.open("/style.css", "r");
    server.streamFile(file, "text/css");
    file.close();
  });

  // Explicitly serve the JavaScript file
  server.on("/script.js", []() {
    File file = LittleFS.open("/script.js", "r");
    server.streamFile(file, "application/javascript");
    file.close();
  });

  // Serve the live sensor data JSON
  server.on("/data", handleData); 
  
  // Turn the server on
  server.begin();                         
}

void loop() {
  // 1. ALWAYS LISTEN FOR THE PHONE
  server.handleClient(); 

  unsigned long currentMillis = millis();

  // 2. CHECK SENSORS (Every 100ms)
  if (currentMillis - previousSensorMillis >= sensorInterval) {
    previousSensorMillis = currentMillis;

    isFrontBlocked = (digitalRead(IR_FRONT_PIN) == 0);

    digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH, 15000); 
    currentDistanceCm = duration * 0.034 / 2;
    if (currentDistanceCm == 0) currentDistanceCm = 999; 
  }

  // 3. NON-BLOCKING BUZZER & LED LOGIC
  if (currentDistanceCm <= 5 || isFrontBlocked) {
    // CRITICAL: Solid tone, no pulsing
    digitalWrite(RED_LED, HIGH); 
    digitalWrite(GREEN_LED, LOW);
    tone(BUZZER_PIN, 1500);
  } 
  else if (currentDistanceCm <= 10) {
    // WARNING: Fast Pulse (Every 100ms)
    digitalWrite(RED_LED, HIGH); 
    digitalWrite(GREEN_LED, LOW);
    if (currentMillis - previousBuzzerMillis >= 100) {
      previousBuzzerMillis = currentMillis;
      buzzerIsOn = !buzzerIsOn; // Flip the buzzer state
      if (buzzerIsOn) tone(BUZZER_PIN, 1000);
      else noTone(BUZZER_PIN);
    }
  } 
  else if (currentDistanceCm <= 15) {
    // NOTICE: Slow Pulse (Every 500ms)
    digitalWrite(RED_LED, HIGH); 
    digitalWrite(GREEN_LED, LOW);
    if (currentMillis - previousBuzzerMillis >= 500) {
      previousBuzzerMillis = currentMillis;
      buzzerIsOn = !buzzerIsOn; // Flip the buzzer state
      if (buzzerIsOn) tone(BUZZER_PIN, 1000);
      else noTone(BUZZER_PIN);
    }
  } 
  else {
    // SAFE: All clear
    digitalWrite(RED_LED, LOW); 
    digitalWrite(GREEN_LED, HIGH);
    noTone(BUZZER_PIN);
  }
}
