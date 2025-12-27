
# Traffic Light Queue System

**Implementation of traffic light problem using Queue data structures**

[![C++](https://img.shields.io/badge/Language-C++-blue.svg)](https://isocpp.org/)
[![SDL2](https://img.shields.io/badge/Graphics-SDL2-green.svg)](https://www.libsdl.org/)


<img width="813" height="857" alt="image" src="https://github.com/user-attachments/assets/a50d0bbb-c3e3-4489-a9be-4704d77c2808" />


## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Compilation](#compilation)
- [Running the Simulation](#running-the-simulation)
- [Data Structures](#data-structures)
- [Configuration](#configuration)
- [Contributors](#contributors)

---

##  Overview

This project implements a traffic junction management system that simulates a four-way intersection using **Queue** and **Priority Queue** data structures. The system manages vehicle flow through multiple lanes with dynamic priority-based traffic light control.

### Problem Statement

A traffic junction connects two major roads, forming a central point where vehicles must navigate through one of three alternative paths. The system handles:

- **Normal Condition**: Vehicles are served equally from each lane using fair distribution
- **High-Priority Condition**: When a priority road (AL2) accumulates more than 10 waiting vehicles, it receives preferential treatment until the count drops below 5

### Solution Approach
    
  The implementation uses:
  - **Queue data structure** to manage vehicles waiting in each lane
- **Priority Queue (Max Heap)** to determine which lane should be served next
- **File-based Inter-Process Communication** between vehicle generator and simulator
- **SDL2 graphics library** for real-time visual simulation

---
  
##  Features

### Core Functionality
-  **Queue-based vehicle management** with O(1) enqueue/dequeue operations
- **Priority queue implementation** using max heap structure
-  **Dynamic traffic light control** - only one lane green at a time
- **Fair distribution algorithm** for normal traffic conditions
-  **Priority lane activation** when AL2 exceeds 10 vehicles
- **Automatic priority deactivation** when AL2 drops below 5 vehicles
  
### Visual Simulation (SDL2)
-  **Real-time graphical interface** with smooth animations
-  **Animated vehicle movement** from entry to exit
-  **Working traffic lights** positioned beside their lanes
-  **Statistics panel** showing lane status and vehicle counts
-  **Priority indicators** highlighting when AL2 is in priority mode
-  **Color-coded vehicles** with unique identifiers

### Traffic Generation
-  **Random vehicle generation** with 8-character alphanumeric IDs
- **Realistic timing** with 1-3 second intervals between vehicles
- **File-based communication** using CSV format
-  **Random lane assignment** for natural traffic distribution

---

##  Installation

### 1. Install SDL2 Libraries

```bash
sudo dnf install SDL2-devel SDL2_ttf-devel gcc-c++ make
```

### 2. Clone the Repository

```bash
git clone https://github.com/MaxPayne-Exertion/Traffic-Light-Queue-System.git
cd Traffic-Light-Queue-System
```

### 3. Verify Installation

```bash
# Check g++ compiler
g++ --version

# Check SDL2
pkg-config --modversion sdl2
pkg-config --modversion SDL2_ttf
```

---

## Compilation

### Using the Run Script 
```bash
chmod +x run.sh
./run.sh
```

The script will automatically:
1. Compile both programs if needed
2. Clean old lane files
3. Start the traffic generator in background
4. Launch the SDL2 simulator

---

##  Running the Simulation

### Quick Start

```bash
./run.sh
```

### Running Components Separately

**Terminal 1 - Traffic Generator:**
```bash
./traffic_generator
```

**Terminal 2 - Simulator:**
```bash
./simulation        # Console version


```
Traffic-Light-Queue-System/
│
├──queue.h                   # queue implementation 
├── simulation.cpp           # main simulation program with SDL2 graphics
├── traffic_generator.cpp    # vehicle generation program
├── run.sh                   # execution script
│
├── lanea.txt               # Lane A vehicle queue (generated at runtime)
├── laneb.txt               # Lane B vehicle queue (generated at runtime)
├── lanec.txt               # Lane C vehicle queue (generated at runtime)
├── laned.txt               # Lane D vehicle queue (generated at runtime)
│
├── README.md               # This file


### File Descriptions

|`queue.h`| Base implementation of the queue along with lanes and vehicles|C++ Header|
| `simulation.cpp` | Main simulator with queue logic and SDL2 visualization | C++ Source |
| `traffic_generator.cpp` | Generates random vehicles and writes to lane files | C++ Source |
| `run.sh` | Automated script to compile and run both programs | Shell Script |
| `lane*.txt` | Stores vehicle data in CSV format (ID, Time, Lane) | Data Files |

---

##  Data Structures

### 1. Queue (Linked List Implementation)

**Purpose**: Store vehicles waiting in each lane

**Structure**:
```cpp
template <typename T>
class Queue {
    Node* front;
    Node* rear;
    int count;
    
    void enqueue(T data);      // O(1)
    T dequeue();               // O(1)
    bool isEmpty();            // O(1)
    int size();                // O(1)
}
```

**Operations**:
- `enqueue()`: Add vehicle to end of queue - **O(1)**
- `dequeue()`: Remove vehicle from front - **O(1)**
- `peek()`: View front vehicle without removing - **O(1)**
- `isEmpty()`: Check if queue is empty - **O(1)**
- `size()`: Get current number of vehicles - **O(1)**

### 2. Priority Queue (Max Heap Implementation)

**Purpose**: Manage lane priorities to determine processing order

**Structure**:
```cpp
class LanePriorityQueue {
    Lane** lanes;
    int capacity;
    int count;
    
    void insert(Lane* lane);        // O(log k)
    Lane* extractMax();             // O(log k)
    void updatePriorities();        // O(k log k)
}
```

**Operations**:
- `insert()`: Add lane to heap - **O(log k)** where k = 4 lanes
- `extractMax()`: Get highest priority lane - **O(log k)**
- `heapifyUp()`: Maintain heap property upward - **O(log k)**
- `heapifyDown()`: Maintain heap property downward - **O(log k)**
- `updatePriorities()`: Rebuild heap after priority changes - **O(k log k)**

### 3. Supporting Data Structures

**Vehicle Class**:
```cpp
class Vehicle {
    string id;              // 8-character unique identifier
    time_t arrivalTime;     // Timestamp of arrival
    string lane;            // Lane assignment (AL2, BL2, CL2, DL2)
}
```

**Lane Class**:
```cpp
class Lane {
    string name;            // Lane identifier
    Queue<Vehicle>* queue;  // Vehicle queue
    int priority;           // Priority level (0 or 100)
    bool isPriorityLane;    // True for AL2
}
```

---

### Fair Distribution Formula

For normal lanes (when no priority is active):

```
vehicles_to_serve = (1/n) × Σ|Li|

Where:
  n = number of normal priority lanes
  |Li| = vehicle count in lane i
```

**Example**:
- AL2: 3 vehicles (normal)
- BL2: 5 vehicles (normal)
- CL2: 7 vehicles (normal)
- DL2: 2 vehicles (normal)

Total = 17 vehicles, n = 4 lanes
Average = 17/4 = 4.25 ≈ 4 vehicles per lane

### Priority Activation Logic

```
IF AL2.vehicle_count > 10 THEN
    AL2.priority ← 100
    SERVE AL2 until AL2.vehicle_count < 5
ELSE
    AL2.priority ← 0
    SERVE using fair distribution
END IF
```

---


### Key Visual Elements

1. **Four-Way Junction**: Central intersection with roads extending in all directions
2. **Three Lanes per Road**: Each road has 3 distinct lanes with yellow markings
3. **Traffic Lights**: Positioned beside their respective lanes
   - Red/Green lights with realistic appearance
   - Only one lane has green light at any time
4. **Animated Vehicles**: 
   - Enter from screen edge
   - Travel through their lane
   - Cross junction when green
   - Exit off opposite edge
   - Color-coded with unique IDs
5. **Statistics Panel**: Real-time information display
   - Vehicle counts per lane
   - Priority indicator [P] for AL2
   - Current green lane
   - Total vehicles processed


### Acknowledgments

- SDL2 Library - Graphics rendering
- Course instructors - Problem statement and guidance
- Online resources - Algorithm optimization techniques
