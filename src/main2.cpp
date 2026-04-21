#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

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
    Serial.println("\n[SYSTEM] Kernel Simulator Active.");
}

void loop() {
    webSocket.loop(); 

    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 2000) { 
        lastUpdate = millis();

        StaticJsonDocument<1024> doc;
        doc["uptime"] = millis() / 1000;
       
        doc["heap"] = ESP.getFreeHeap(); 
        
        JsonArray array = doc.createNestedArray("processes");

        for (int i = 0; i < 3; i++) {
            // --- DAY 5: SIMULATION LOGIC ---
            // Randomly toggle between RUNNING and BLOCKED
            processes[i].state = (random(0, 10) > 7) ? BLOCKED : RUNNING;
            
            if (processes[i].state == RUNNING) {
                processes[i].cpu = random(10, 60);  // Simulating CPU load
                processes[i].mem = random(1024, 4096); // Simulating memory flux
            } else {
                processes[i].cpu = 0;
            }

            JsonObject p = array.createNestedObject();
            p["pid"] = processes[i].pid;
            p["name"] = processes[i].name;
            p["state"] = (processes[i].state == RUNNING) ? "RUNNING" : "BLOCKED";
            p["cpu"] = processes[i].cpu;
            p["mem"] = processes[i].mem;
        }

        String output;
        serializeJson(doc, output);
        webSocket.broadcastTXT(output); 
        Serial.println("[SIMULATOR] Process states updated and broadcasted.");
    }
}