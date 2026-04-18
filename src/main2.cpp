#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>

WebSocketsServer webSocket = WebSocketsServer(81);

const char* ssid = "Daksh_hotspot";    
const char* password = "dddd1234";  


// Defining the possible states of a process
enum State { READY, RUNNING, BLOCKED, KILLED };

// The Process Control Block (PCB) Structure
struct ProcessControlBlock {
    int pid;            // Process ID
    const char* name;   // Process Name
    State state;        // Current State
    uint32_t mem;       // Memory usage (KB)
    int cpu;            // CPU usage (%)
};

// Initializing 3 Simulated Kernel Processes
ProcessControlBlock processes[3] = {
    {1, "Sensor_P1", READY, 1024, 0},
    {2, "Network_P2", READY, 2048, 0},
    {3, "Calc_P3", BLOCKED, 512, 0}
};

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_CONNECTED) {
        Serial.printf("[KERNEL] Dashboard connected [%u]\n", num);
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\n[SYSTEM] Network Layer Online");
    
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
    
    
    Serial.println("[SYSTEM] Kernel PCB structures initialized for 3 tasks.");
}

void loop() {
    webSocket.loop(); 
}