#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// Initialize the Server on Port 81
WebSocketsServer webSocket = WebSocketsServer(81);

const char* ssid = "Daksh_hotspot";    
const char* password = "dddd1234";  

// --- OS STRUCTURES ---
enum State { READY, RUNNING, BLOCKED, KILLED };

struct ProcessControlBlock {
    int pid;
    const char* name;
    State state;
    uint32_t mem;
    int cpu;
};

// Our 3 Simulated Processes
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
            int idx = targetPid - 1;

            if (command == "KILL") {
                processes[idx].state = KILLED;
                processes[idx].mem = 0;
                processes[idx].cpu = 0;
            } 
            else if (command == "BLOCK") {
                processes[idx].state = BLOCKED;
                processes[idx].cpu = 0;
            } 
            else if (command == "RESUME") {
                processes[idx].state = RUNNING; // Set back to active
            }
        }
    }
}


void setup() {
    Serial.begin(115200);
    delay(1000);

    // Connect to Wi-Fi
    Serial.printf("\n[SYSTEM] Connecting to %s", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\n[SYSTEM] WiFi Connected!");
    Serial.print("[SYSTEM] IP Address: ");
    Serial.println(WiFi.localIP()); // IMPORTANT: Note this IP for your Dashboard!

    // Start WebSocket
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
    
    Serial.println("[SYSTEM] Kernel Sentinel Active on Port 81");
}

void loop() {
    webSocket.loop(); 

    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 2000) { 
        lastUpdate = millis();

        uint32_t simulatedUsedMemory = 0;

        for (int i = 0; i < 3; i++) {
            if (processes[i].state != KILLED) {
                // Randomize states
                processes[i].state = (random(0, 10) > 8) ? BLOCKED : RUNNING;
                
                if (processes[i].state == RUNNING) {
                    processes[i].cpu = random(15, 45);
                    // Give processes BIGGER memory footprints to see them on the graph
                    processes[i].mem = random(2000, 8000); 
                    simulatedUsedMemory += processes[i].mem;
                } else {
                    processes[i].cpu = 0;
                }
            }
        }

        // Calculate a "True" Dynamic Heap for the Dashboard
        // Internal Free Heap - what our processes are "using"
        uint32_t dynamicHeap = ESP.getFreeHeap() - simulatedUsedMemory;

        StaticJsonDocument<1024> doc;
        doc["heap"] = dynamicHeap; // Sending the impacted heap value
        doc["uptime"] = millis() / 1000;
        
        JsonArray array = doc.createNestedArray("processes");
        for (int i = 0; i < 3; i++) {
            JsonObject p = array.createNestedObject();
            p["pid"] = processes[i].pid;
            p["name"] = processes[i].name;
            p["state"] = (processes[i].state == RUNNING ? "RUNNING" : 
                          (processes[i].state == BLOCKED ? "BLOCKED" : 
                          (processes[i].state == KILLED ? "KILLED" : "READY")));
            p["cpu"] = processes[i].cpu;
            p["mem"] = processes[i].mem;
        }

        String output;
        serializeJson(doc, output);
        webSocket.broadcastTXT(output); 
    }
}