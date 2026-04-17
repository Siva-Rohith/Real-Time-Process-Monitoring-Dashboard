#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
// --- WIFI SETTINGS ---
const char* ssid = "Daksh_hotspot";    
const char* password = "dddd1234";  

WebSocketsServer webSocket = WebSocketsServer(81);

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_CONNECTED:
            Serial.printf("[KERNEL] Admin Dashboard connected on channel [%u]\n", num);
            break;
        case WStype_DISCONNECTED:
            Serial.printf("[KERNEL] Admin Dashboard disconnected\n");
            break;
        case WStype_TEXT:
            // This will handle our 'KILL:1' or 'BLOCK:2' commands later
            Serial.printf("[SIGNAL] Received from Admin: %s\n", payload);
            break;
    }
}

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

    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
    Serial.println("[SYSTEM] Communication Protocol (WebSocket) Initialized on Port 81");

}

void loop() {
    webSocket.loop();
    // delay(10); 
}