RTOS Kernel Sentinel: Real-Time Process Monitoring & Management
An Operating System project implementing a micro-kernel simulator on the ESP8266. This system bridges the gap between low-level hardware resource management and high-level administrative telemetry using a real-time WebSocket dashboard.

🚀 Overview
The Kernel Sentinel is designed to demonstrate core OS concepts such as process state management, resource allocation tracking, and inter-process communication (IPC). The system utilizes an ESP8266 (NodeMCU) to simulate a multi-tasking environment where an administrator can monitor and control system health remotely.

🧠 Core Operating System Concepts
This project implements several fundamental OS pillars:

Process Control Block (PCB): A dedicated data structure within the kernel that tracks task metadata including PID, Process Name, Execution State, and Memory Footprint.

Process State Machine: Tasks transition through lifecycle states: READY → RUNNING → BLOCKED → KILLED.

Resource Management: Real-time telemetry of Global Free Heap and Per-Process CPU utilization.

Kernel Signals: Implementation of remote system calls (Suspend/Resume/Kill) sent from the Administrative Console to the Hardware Kernel.

🛠️ Technical Stack
Hardware: ESP8266 (NodeMCU)

Kernel Language: C++ (Arduino/PlatformIO)

Communication: Full-Duplex WebSockets (Port 81)

Frontend Dashboard: HTML5, Bootstrap 5, JavaScript (ES6)

Data Serialization: ArduinoJson v6 for efficient telemetry packaging.

📊 Key Features
Administrative Process Control: A live-updating table allowing for granular control over simulated kernel tasks.

Heap Telemetry: A real-time graphical representation of system memory usage, identifying potential fragmentation or leaks.

Global System Gauges: Instant visual feedback on overall CPU load and total available heap.

Kernel Event Log: A timestamped history of administrative commands and system heartbeats.
