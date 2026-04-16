#include <Arduino.h>
#include <ESP8266WiFi.h>

// --- WIFI SETTINGS ---
const char* ssid = "Daksh_hotspot";    
const char* password = "dddd1234";  

void setup() {
  
    Serial.begin(115200);
    delay(1000);

    // Initial System Message
    Serial.println("\n[SYSTEM] RTOS Sentinel Kernel Initializing...");
    Serial.printf("[SYSTEM] Attempting to connect to SSID: %s\n", ssid);

   
    WiFi.begin(ssid, password);
    
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // Success Message
    Serial.println("\n[SYSTEM] Network Layer Established!");
    Serial.print("[SYSTEM] IP Address assigned: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    
    delay(10); 
}