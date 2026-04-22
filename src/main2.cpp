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
    if (type == WStype_TEXT) {
        String msg = (char*)payload;
        int sepIndex = msg.indexOf(':');
        
        if (sepIndex != -1) {
            String command = msg.substring(0, sepIndex);
            int targetPid = msg.substring(sepIndex + 1).toInt();
            int idx = targetPid - 1; // Array index is PID - 1

            if (command == "KILL" && idx >= 0 && idx < 3) {
                processes[idx].state = KILLED;
                processes[idx].mem = 0;
                processes[idx].cpu = 0;
                Serial.printf("[KERNEL] SIGKILL received for PID %d\n", targetPid);
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
    Serial.println("\n[SYSTEM] Kernel Command Layer (Day 6) Active.");
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
            // Day 6: Only update simulation if process isn't KILLED
            if (processes[i].state != KILLED) {
                processes[i].state = (random(0, 10) > 8) ? BLOCKED : RUNNING;
                if (processes[i].state == RUNNING) {
                    processes[i].cpu = random(15, 45);
                    processes[i].mem = random(2000, 8000); 
                } else {
                    processes[i].cpu = 0;
                }
            }

            JsonObject p = array.createNestedObject();
            p["pid"] = processes[i].pid;
            p["name"] = processes[i].name;
            p["state"] = (processes[i].state == RUNNING ? "RUNNING" : 
                          (processes[i].state == BLOCKED ? "BLOCKED" : "KILLED"));
            p["cpu"] = processes[i].cpu;
            p["mem"] = processes[i].mem;
        }

        String output;
        serializeJson(doc, output);
        webSocket.broadcastTXT(output); 
    }
}