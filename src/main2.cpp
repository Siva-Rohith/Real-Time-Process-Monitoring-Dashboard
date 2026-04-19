#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h> // Day 4: New Library for Data Packaging

WebSocketsServer webSocket = WebSocketsServer(81);

const char* ssid = "Daksh_hotspot";    
const char* password = "dddd1234";  

enum State { READY, RUNNING, BLOCKED, KILLED };

struct ProcessControlBlock {
    int pid;
    const char* name;
    State state;
    uint32_t mem;
    int cpu;
};

ProcessControlBlock processes[3] = {
    {1, "Sensor_P1", READY, 1024, 0},
    {2, "Network_P2", READY, 2048, 0},
    {3, "Calc_P3", BLOCKED, 512, 0}
};

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_CONNECTED) Serial.printf("[KERNEL] Dashboard Link Active\n");
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
    Serial.println("\n[SYSTEM] Telemetry Layer (JSON) Initialized.");
}

void loop() {
    webSocket.loop(); 

    // Day 4: Send Kernel Heartbeat every 2 seconds
    static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 2000) { 
        lastHeartbeat = millis();

        StaticJsonDocument<1024> doc;
        doc["uptime"] = millis() / 1000;
        
        JsonArray array = doc.createNestedArray("processes");
        for (int i = 0; i < 3; i++) {
            JsonObject p = array.createNestedObject();
            p["pid"] = processes[i].pid;
            p["name"] = processes[i].name;
            p["state"] = "READY";
            p["cpu"] = 25;       
        }

        String output;
        serializeJson(doc, output);
        webSocket.broadcastTXT(output); // Send to Dashboard
        Serial.println("[HEARTBEAT] Telemetry Packet Broadcasted");
    }
}