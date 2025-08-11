# 🖥️ System Monitor (C++ & Dear ImGui)

A **Desktop System Monitor** built in **C++** with a modern UI using [Dear ImGui](https://github.com/ocornut/imgui).  
The application monitors your computer’s **CPU, Memory, SWAP, Fan, Thermal, Disk, and Network** in real time using Linux's `/proc` filesystem.

---

##  Objectives
The project demonstrates:
- Proficiency in **programming logic**.
- Ability to adapt to a **new programming language** (C++).
- Familiarity with **UI/UX development** using Dear ImGui.
- Understanding of **Linux system resource monitoring**.

---

##  Features

### **System Information**
- OS type
- Logged-in user
- Hostname
- Process statistics (running, sleeping, zombie, etc.)
- CPU type

### **CPU, Fan, Thermal Tabs**
- Real-time performance graphs
- Toggle animation on/off
- Adjustable FPS (slider)
- Adjustable Y-scale (slider)
- CPU % overlay
- Fan status, speed, and level
- Thermal temperature reading

### **Memory & Processes**
- Physical Memory (RAM) usage (visual display)
- Virtual Memory (SWAP) usage (visual display)
- Disk usage (visual display)
- Process table with:
  - PID
  - Name
  - State
  - CPU usage %
  - Memory usage %
- Process filtering
- Multi-row selection

### **Network**
- List network interfaces (IPv4)
- RX & TX tables:
  - RX: bytes, packets, errs, drop, fifo, frame, compressed, multicast
  - TX: bytes, packets, errs, drop, fifo, colls, carrier, compressed
- Visual progress bars for RX & TX
- Automatic byte-to-KB/MB/GB conversion
- Range scaling (0GB → 2GB)

---

##  Project Structure
```bash
system-monitor/
├── header.h
├── imgui/ # Dear ImGui core files
│ └── lib/
│ ├── backend/ # ImGui backend (SDL + OpenGL)
│ ├── gl3w/ # OpenGL loader
│ ├── *.cpp / *.h # ImGui source files
├── main.cpp # Main loop & rendering
├── Makefile
├── mem.cpp # Memory & process logic
├── network.cpp # Network monitoring logic
└── system.cpp # System resource logic

```
---

##  Installation

### **Prerequisites**
- Linux OS
- C++17 or later
- `make`
- `g++` or `clang++`
- SDL2 development package

Install dependencies:
```bash
sudo apt update
sudo apt install build-essential libsdl2-dev
```
##  Build & Run
#### 1. Clone the repository
```bash
git clone https://github.com/yourusername/system-monitor.git
cd system-monitor
```
### 2. Build the project
```bash
make
```

### 3. Run the application
```bash
./system-monitor
````

⚙️ How It Works

    Uses /proc filesystem for real-time resource data.

    Uses Dear ImGui in immediate mode for UI rendering.

    Uses SDL2 + OpenGL3 backend for rendering and event handling.


## License

This project is licensed under the MIT [License](/License)

## Contributions
Pull requests are welcome! If you have improvements for:

    New visualizations

    More accurate monitoring

    UI enhancements
