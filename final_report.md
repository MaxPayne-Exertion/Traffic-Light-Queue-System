# Traffic Light Queue System - Project Report

**Data Structures and Algorithms (COMP202)**  
**Assignment #1**

---

## Student Information

- **Name**: SHASHANK HARI JOSHI
- **Roll Number**: 28
- **Course**: COMP202 - Data Structures and Algorithms
- **Assignment**: #1 - Traffic Light Problem using Queue
- **GitHub Repository**: https://github.com/MaxPayne-Exertion/Traffic-Light-Queue-System

---

## Table of Contents

1. [Executive Summary](#1-executive-summary)
2. [Problem Description](#2-problem-description)
3. [Objectives](#3-objectives)
4. [Summary of Work](#4-summary-of-work)
5. [Data Structures Used](#5-data-structures-used)
6. [Implementation Details](#6-implementation-details)
7. [Algorithm Description](#7-algorithm-description)
8. [Function Documentation](#8-function-documentation)
9. [Time Complexity Analysis](#9-time-complexity-analysis)
10. [Testing and Validation](#10-testing-and-validation)
11. [Results and Observations](#11-results-and-observations)
12. [Challenges and Solutions](#12-challenges-and-solutions)
13. [Conclusion](#13-conclusion)
14. [References](#14-references)

---

## 1. Executive Summary

This project implements a comprehensive traffic junction management system using Queue and Priority Queue data structures to solve a real-world traffic light control problem. The system simulates a four-way intersection where vehicles must be processed efficiently while handling both normal traffic conditions and high-priority scenarios.

### Key Achievements

- **Complete Queue Implementation**: Linked list-based queue with O(1) enqueue/dequeue operations
- **Priority Queue System**: Max heap implementation for dynamic lane prioritization
- **Visual Simulation**: SDL2-based graphical interface with real-time animations
- **Fair Traffic Distribution**: Algorithm ensuring equitable service across all lanes
- **Priority Management**: Automatic activation and deactivation based on vehicle count
- **Inter-Process Communication**: File-based communication between generator and simulator

### Technologies Used

- **Programming Language**: C++ 
- **Graphics Library**: SDL2 and SDL2_ttf
- **Build System**: g++ compiler 
- **Communication**: File-based
- **Version Control**: Git and GitHub

---

## 2. Problem Description

### 2.1 Traffic Junction Scenario

A traffic junction connects two major roads, forming a central point where vehicles must choose one of three alternative paths to continue. The system must efficiently manage traffic flow while ensuring:

1. **Fairness**: In normal conditions, all lanes receive equal service
2. **Priority Handling**: When congestion occurs on a priority road, it receives preferential treatment
3. **Safety**: Only one lane can have a green light at any time to prevent collisions
4. **Efficiency**: Vehicles should experience minimal wait times

### 2.2 Road and Lane Configuration

The junction consists of:
- **4 Major Roads**: A, B, C, D
- **3 Lanes per Road**: 
  - Lane 1 (L1): Incoming lane
  - Lane 2 (L2): Main processing lane (subject to traffic light control)
  - Lane 3 (L3): Free lane (left turns only)

**Lane Designations**:
- **AL2**: Left road, Lane 2 (PRIORITY LANE)
- **BL2**: Top road, Lane 2 (Normal)
- **CL2**: Right road, Lane 2 (Normal)
- **DL2**: Bottom road, Lane 2 (Normal)

### 2.3 Traffic Conditions

#### Normal Condition
All lanes are served equally based on average vehicle count:

```
Vehicles Served = (1/n) × Σ|Li|

Where:
  n = total number of lanes
  |Li| = vehicle count in lane i
```

#### High-Priority Condition
When **AL2** (priority lane) accumulates **more than 10 vehicles**:
1. AL2 is served immediately
2. Processing continues until AL2 has **fewer than 5 vehicles**
3. System returns to normal condition

### 2.4 Traffic Light States

- **State 1 (RED)**: Stop - vehicles must wait
- **State 2 (GREEN)**: Go - vehicles can proceed through junction

**Critical Rule**: Only ONE lane can have a green light at any given time to prevent deadlock.

---

## 3. Objectives

### 3.1 Primary Objectives

1. **Implement Queue Data Structure**
   - Design efficient queue using linked list
   - Achieve O(1) time complexity for basic operations
   - Support generic types using templates

2. **Implement Priority Queue**
   - Design max heap-based priority queue
   - Support dynamic priority updates
   - Maintain heap property efficiently

3. **Create Visual Simulation**
   - Use SDL2 for graphical representation
   - Animate vehicle movement
   - Display real-time statistics
   - Show traffic light states

4. **Establish Communication**
   - Implement file-based IPC
   - Ensure reliable data transfer
   - Handle concurrent file access

### 3.2 Learning Objectives

- Understanding queue operations and applications
- Implementing priority-based scheduling algorithms
- Working with graphics libraries
- Managing concurrent processes
- Analyzing algorithm complexity

---

## 4. Summary of Work

### 4.1 Development Timeline

1. **Phase 1: Data Structure Design**
   - Designed Queue class with linked list
   - Implemented Priority Queue using max heap
   - Created Vehicle and Lane classes
   - Tested basic operations

2. **Phase 2: Traffic Logic Implementation**
   - Implemented fair distribution algorithm
   - Added priority activation logic
   - Developed traffic light control system
   - Implemented vehicle processing

3. **Phase 3: Visual Simulation**
   - Set up SDL2 environment
   - Designed junction layout
   - Implemented vehicle animations
   - Added statistics panel

4. **Phase 4: Integration and Testing**
   - Connected generator with simulator
   - Tested file-based communication
   - Validated priority system
   - Performance optimization

5. **Phase 5: Documentation**
   - Wrote comprehensive README
   - Created detailed report
   - Added inline code comments
   - Prepared submission materials

### 4.2 Key Accomplishments

 **Functional Implementation**: All core features working correctly
 **Visual Quality**: Professional SDL2 graphics with smooth animations
 **Algorithm Correctness**: Priority and fair distribution working as specified
 **Code Quality**: Clean, well-commented, and maintainable code
 **Documentation**: Complete and detailed documentation
 **Testing**: Comprehensive testing with multiple scenarios

---

## 5. Data Structures Used

### 5.1 Queue (Linked List Implementation)

#### Structure
```cpp
template <typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
    };
    
    Node* front;    // Points to first element
    Node* rear;     // Points to last element
    int count;      // Number of elements
    
public:
    Queue();
    ~Queue();
    void enqueue(T data);
    T dequeue();
    T peek();
    bool isEmpty();
    int size();
};
```

#### Purpose
- Store vehicles waiting in each lane
- Maintain FIFO (First-In-First-Out) order
- Efficient insertion and removal

#### Implementation Details
- **Dynamic memory allocation**: Nodes created on heap
- **Pointer manipulation**: Front and rear pointers updated
- **Counter tracking**: Size maintained for O(1) access

#### Operations

| Operation | Time Complexity | Space Complexity | Description |
|-----------|----------------|------------------|-------------|
| `enqueue()` | O(1) | O(1) | Add element to rear |
| `dequeue()` | O(1) | O(1) | Remove element from front |
| `peek()` | O(1) | O(1) | View front element |
| `isEmpty()` | O(1) | O(1) | Check if empty |
| `size()` | O(1) | O(1) | Get element count |

### 5.2 Priority Queue (Max Heap Implementation)

#### Structure
```cpp
class LanePriorityQueue {
private:
    Lane** lanes;           // Array of lane pointers
    int capacity;           // Maximum lanes
    int count;              // Current lanes
    
    void heapifyUp(int index);
    void heapifyDown(int index);
    
public:
    LanePriorityQueue(int cap);
    ~LanePriorityQueue();
    void insert(Lane* lane);
    Lane* extractMax();
    void updatePriorities();
    Lane* peek();
};
```

#### Purpose
- Determine which lane to process next
- Maintain lanes sorted by priority
- Support dynamic priority changes

#### Heap Properties
- **Max Heap**: Parent priority ≥ Children priorities
- **Complete Binary Tree**: Filled left to right
- **Array Representation**: 
  - Parent of i: (i-1)/2
  - Left child of i: 2i+1
  - Right child of i: 2i+2

#### Operations

| Operation | Time Complexity | Space Complexity | Description |
|-----------|----------------|------------------|-------------|
| `insert()` | O(log k) | O(1) | Add lane to heap |
| `extractMax()` | O(log k) | O(1) | Get highest priority |
| `heapifyUp()` | O(log k) | O(1) | Restore heap upward |
| `heapifyDown()` | O(log k) | O(1) | Restore heap downward |
| `updatePriorities()` | O(k log k) | O(1) | Rebuild entire heap |
| `peek()` | O(1) | O(1) | View max priority |

*Note: k = 4 (number of lanes), so effectively O(1) in practice*

### 5.3 Supporting Classes

#### Vehicle Class
```cpp
class Vehicle {
public:
    string id;              // 8-character unique identifier
    time_t arrivalTime;     // Unix timestamp
    string lane;            // Lane assignment
    
    Vehicle(string vid, time_t aTime, string vLane);
};
```

**Purpose**: Represent individual vehicles with tracking information

#### Lane Class
```cpp
class Lane {
public:
    string name;                    // "AL2", "BL2", "CL2", "DL2"
    Queue<Vehicle>* vehicleQueue;   // Waiting vehicles
    int priority;                   // 0 (normal) or 100 (high)
    bool isPriorityLane;            // true for AL2
    
    Lane(string n, bool isPriority);
    int getVehicleCount();
    void updatePriority();
};
```

**Purpose**: Represent each lane with its queue and priority status

### 5.4 Data Structure Relationships

```
LanePriorityQueue
    ↓ (contains)
Lane[4]
    ↓ (each contains)
Queue<Vehicle>
    ↓ (each contains)
Vehicle[n]
```

### 5.5 Summary Table

| Data Structure | Implementation | Purpose | Time Complexity | Space Complexity |
|----------------|----------------|---------|----------------|------------------|
| **Queue** | Linked List | Store vehicles in FIFO order | O(1) enqueue/dequeue | O(n) where n = vehicles |
| **Priority Queue** | Max Heap | Manage lane priorities | O(log k) insert/extract | O(k) where k = 4 lanes |
| **Vehicle** | Class | Represent vehicle data | N/A | O(1) per vehicle |
| **Lane** | Class | Manage lane state | O(1) most ops | O(1) per lane |

---

## 6. Implementation Details

### 6.1 File Structure

The project consists of the following main components:

#### simulation.cpp
**Purpose**: Main simulation program with SDL2 visualization

**Key Components**:
- Queue implementation
- Priority queue implementation
- Vehicle and Lane classes
- SDL2 rendering system
- Traffic light control logic
- Animation system
- Statistics display

**Lines of Code**: ~800-1000 lines

#### traffic_generator.cpp
**Purpose**: Generate random vehicles and write to lane files

**Key Functions**:
- `generateVehicleID()`: Create 8-character alphanumeric IDs
- `main()`: Generate vehicles at random intervals
- Random lane assignment
- File I/O operations

**Lines of Code**: ~100-150 lines

#### run.sh
**Purpose**: Automated script to compile and run both programs

**Functions**:
- Check for compiled binaries
- Compile if needed
- Clean old lane files
- Start generator in background
- Launch simulator
- Handle cleanup on exit

### 6.2 Communication Protocol

#### File-Based 

**Format**: 
```
VehicleID,ArrivalTime,LaneName
```

**Example**:
```
A3b8X9mQ,1735301234,AL2
K7n2Pw4S,1735301237,BL2
Zm5Vt1Jx,1735301240,CL2
```

**Files**:
- `lanea.txt`: AL2 vehicles
- `laneb.txt`: BL2 vehicles
- `lanec.txt`: CL2 vehicles
- `laned.txt`: DL2 vehicles

**Process**:
1. Generator writes vehicle data to appropriate file
2. Simulator reads files every 2 seconds
3. Vehicles are parsed and enqueued
4. Files are cleared after reading

**Advantages**:
- Simple to implement
- Easy to debug (can inspect files)
- No complex setup required

**Disadvantages**:
- Latency (~2 seconds)
- File I/O overhead
- Not suitable for high-frequency updates

### 6.3 SDL2 Implementation

#### Window Setup
```cpp
SDL_Init(SDL_INIT_VIDEO);
TTF_Init();

window = SDL_CreateWindow(
    "Traffic Light Queue System",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    SDL_WINDOW_SHOWN
);

renderer = SDL_CreateRenderer(
    window,
    -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
);
```

#### Rendering Pipeline
1. **Clear screen** with background color
2. **Draw roads** (horizontal and vertical)
3. **Draw lane markings** (dashed yellow lines)
4. **Draw junction center**
5. **Draw traffic lights** with current states
6. **Draw animated vehicles** at current positions
7. **Draw statistics panel** with current data
8. **Present frame** to display

#### Animation System

**Vehicle Animation**:
```cpp
struct AnimatedVehicle {
    float x, y;              // Current position
    float targetX, targetY;  // Destination
    bool isMoving;           // Animation state
};

// Update each frame
void updateAnimations(float deltaTime) {
    for (auto& vehicle : vehicles) {
        if (vehicle.isMoving) {
            // Calculate direction vector
            float dx = vehicle.targetX - vehicle.x;
            float dy = vehicle.targetY - vehicle.y;
            float distance = sqrt(dx*dx + dy*dy);
            
            // Move toward target
            float speed = 250.0f * deltaTime;
            vehicle.x += (dx/distance) * speed;
            vehicle.y += (dy/distance) * speed;
            
            // Check if reached
            if (distance < 5.0f) {
                vehicle.isMoving = false;
            }
        }
    }
}
```

### 6.4 Traffic Light Control

#### State Management
```cpp
enum LightState { RED = 0, GREEN = 1 };

map<string, LightState> trafficLights;

void setTrafficLight(string lane, LightState state) {
    if (state == GREEN) {
        // Set all lights to RED first
        for (auto& light : trafficLights) {
            light.second = RED;
        }
        // Then set requested lane to GREEN
        trafficLights[lane] = GREEN;
        currentGreenLane = lane;
    }
}
```

**Ensures**: Only one lane has green light at any time

### 6.5 Priority Logic

#### Priority Activation
```cpp
void Lane::updatePriority() {
    if (isPriorityLane && vehicleQueue->size() > 10) {
        priority = 100;  // High priority
    } else {
        priority = 0;    // Normal priority
    }
}
```

#### Vehicle Processing
```cpp
int calculateVehiclesToServe(Lane* lane) {
    if (lane->priority > 0) {
        // Priority mode: serve until < 5 vehicles
        return max(0, lane->getVehicleCount() - 5);
    } else {
        // Normal mode: fair distribution
        int total = 0, count = 0;
        for each normal_lane {
            total += normal_lane->getVehicleCount();
            count++;
        }
        return (count > 0) ? (total / count) : 1;
    }
}
```

---

## 7. Algorithm Description

### 7.1 Main Simulation Algorithm

```
ALGORITHM TrafficSimulation

INPUT: None (continuous operation)
OUTPUT: Visual simulation with processed vehicles

INITIALIZE:
    lanes ← [AL2(priority=true), BL2, CL2, DL2]
    priorityQueue ← MaxHeap(lanes)
    trafficLights ← {AL2: RED, BL2: RED, CL2: RED, DL2: RED}
    vehiclesProcessed ← 0
    lastLoadTime ← 0
    lastUpdateTime ← 0
    lastProcessTime ← 0

WHILE simulation running:
    currentTime ← getCurrentTime()
    deltaTime ← currentTime - lastFrameTime
    
    // 1. LOAD NEW VEHICLES (every 2 seconds)
    IF currentTime - lastLoadTime > 2000:
        FOR each laneFile in [lanea.txt, laneb.txt, lanec.txt, laned.txt]:
            vehicles ← readCSV(laneFile)
            FOR each vehicle in vehicles:
                lanes[vehicle.lane].enqueue(vehicle)
            clearFile(laneFile)
        lastLoadTime ← currentTime
    
    // 2. UPDATE PRIORITIES (every 1 second)
    IF currentTime - lastUpdateTime > 1000:
        FOR each lane in lanes:
            lane.updatePriority()
        priorityQueue.rebuild()
        lastUpdateTime ← currentTime
    
    // 3. PROCESS LANES (every 3 seconds)
    IF currentTime - lastProcessTime > 3000:
        IF NOT priorityQueue.isEmpty():
            currentLane ← priorityQueue.extractMax()
            
            IF currentLane.hasVehicles():
                vehiclesToServe ← calculateVehiclesToServe(currentLane)
                setTrafficLight(currentLane, GREEN)
                
                FOR i ← 1 TO vehiclesToServe:
                    vehicle ← currentLane.dequeue()
                    createAnimation(vehicle, currentLane)
                    vehiclesProcessed ← vehiclesProcessed + 1
                
                setTrafficLight(currentLane, RED)
            
            priorityQueue.insert(currentLane)
        lastProcessTime ← currentTime
    
    // 4. UPDATE ANIMATIONS (every frame)
    updateAnimations(deltaTime)
    removeOffScreenVehicles()
    
    // 5. RENDER
    clearScreen()
    drawRoads()
    drawJunction()
    drawTrafficLights()
    drawVehicles()
    drawStatistics()
    presentFrame()
    
    // 6. HANDLE EVENTS
    handleUserInput()
    
    lastFrameTime ← currentTime
    wait(16ms)  // ~60 FPS

END WHILE

END ALGORITHM
```

### 7.2 Fair Distribution Algorithm

```
ALGORITHM CalculateVehiclesToServe

INPUT: lane (Lane object)
OUTPUT: number of vehicles to serve (integer)

IF lane.priority > 0:
    // Priority mode
    vehiclesToServe ← lane.vehicleCount - 5
    IF vehiclesToServe < 0:
        vehiclesToServe ← 0
    RETURN vehiclesToServe

ELSE:
    // Normal mode - fair distribution
    totalVehicles ← 0
    normalLaneCount ← 0
    
    FOR each l in [AL2, BL2, CL2, DL2]:
        IF l.priority = 0:
            totalVehicles ← totalVehicles + l.vehicleCount
            normalLaneCount ← normalLaneCount + 1
    
    IF normalLaneCount > 0:
        average ← totalVehicles / normalLaneCount
        vehiclesToServe ← floor(average)
        
        IF vehiclesToServe > lane.vehicleCount:
            vehiclesToServe ← lane.vehicleCount
        
        IF vehiclesToServe = 0 AND lane.vehicleCount > 0:
            vehiclesToServe ← 1  // Serve at least one
        
        RETURN vehiclesToServe
    ELSE:
        RETURN 0

END ALGORITHM
```

**Example Calculation**:

Scenario:
- AL2: 3 vehicles (normal, priority = 0)
- BL2: 5 vehicles (normal, priority = 0)
- CL2: 7 vehicles (normal, priority = 0)
- DL2: 2 vehicles (normal, priority = 0)

Calculation:
```
Total = 3 + 5 + 7 + 2 = 17 vehicles
Normal lanes = 4
Average = 17 / 4 = 4.25
Floor(4.25) = 4 vehicles per lane
```

Result: Each lane serves 4 vehicles (or less if queue is smaller)

### 7.3 Priority Queue Operations

#### Insert Operation
```
ALGORITHM Insert(lane)

INPUT: lane (Lane pointer)
OUTPUT: None

lanes[count] ← lane
count ← count + 1
heapifyUp(count - 1)

END ALGORITHM
```

#### HeapifyUp Operation
```
ALGORITHM HeapifyUp(index)

INPUT: index (integer)
OUTPUT: None (modifies heap in-place)

IF index = 0:
    RETURN

parentIndex ← (index - 1) / 2

IF lanes[parentIndex].priority < lanes[index].priority:
    swap(lanes[parentIndex], lanes[index])
    heapifyUp(parentIndex)

END ALGORITHM
```

#### ExtractMax Operation
```
ALGORITHM ExtractMax()

INPUT: None
OUTPUT: Lane with highest priority

IF count = 0:
    ERROR "Priority queue is empty"

maxLane ← lanes[0]
lanes[0] ← lanes[count - 1]
count ← count - 1
heapifyDown(0)

RETURN maxLane

END ALGORITHM
```

#### HeapifyDown Operation
```
ALGORITHM HeapifyDown(index)

INPUT: index (integer)
OUTPUT: None (modifies heap in-place)

leftChild ← 2 * index + 1
rightChild ← 2 * index + 2
largest ← index

IF leftChild < count AND lanes[leftChild].priority > lanes[largest].priority:
    largest ← leftChild

IF rightChild < count AND lanes[rightChild].priority > lanes[largest].priority:
    largest ← rightChild

IF largest ≠ index:
    swap(lanes[index], lanes[largest])
    heapifyDown(largest)

END ALGORITHM
```

### 7.4 Vehicle Animation Algorithm

```
ALGORITHM AnimateVehicle(vehicle, lane)

INPUT: vehicle (Vehicle object), lane (string)
OUTPUT: AnimatedVehicle added to animation list

// Determine start and end positions based on lane
SWITCH lane:
    CASE "AL2":  // Left to right
        startX ← 50
        startY ← centerY - junctionHalf + LANE_WIDTH * 1.5
        endX ← WINDOW_WIDTH + 50
        endY ← startY
    CASE "BL2":  // Top to bottom
        startX ← centerX - junctionHalf + LANE_WIDTH * 1.5
        startY ← 50
        endX ← startX
        endY ← WINDOW_HEIGHT + 50
    CASE "CL2":  // Right to left
        startX ← WINDOW_WIDTH - 50
        startY ← centerY + junctionHalf - LANE_WIDTH * 1.5
        endX ← -50
        endY ← startY
    CASE "DL2":  // Bottom to top
        startX ← centerX + junctionHalf - LANE_WIDTH * 1.5
        startY ← WINDOW_HEIGHT - 50
        endX ← startX
        endY ← -50

animatedVehicle ← new AnimatedVehicle(
    vehicle.id,
    startX, startY,
    lane,
    direction
)
animatedVehicle.targetX ← endX
animatedVehicle.targetY ← endY
animatedVehicle.isMoving ← true

animationList.add(animatedVehicle)

END ALGORITHM
```

---

## 8. Function Documentation

### 8.1 Queue Operations

#### enqueue()
```cpp
void Queue<T>::enqueue(T data)
```
**Purpose**: Add element to the rear of the queue

**Parameters**: 
- `data`: Element to be added

**Time Complexity**: O(1)

**Algorithm**:
1. Create new node with data
2. If queue empty, set front and rear to new node
3. Else, link rear->next to new node and update rear
4. Increment count

**Example**:
```cpp
Queue<Vehicle> q;
Vehicle v("ABC123", time(0), "AL2");
q.enqueue(v);  // v added to queue
```

#### dequeue()
```cpp
T Queue<T>::dequeue()
```
**Purpose**: Remove and return element from front of queue

**Returns**: Element at front of queue

**Time Complexity**: O(1)

**Algorithm**:
1. Check if queue is empty (throw exception if true)
2. Store front data
3. Move front pointer to next node
4. Delete old front node
5. Decrement count
6. Return stored data

**Example**:
```cpp
Vehicle v = q.dequeue();  // Returns and removes front vehicle
```

#### peek()
```cpp
T Queue<T>::peek()
```
**Purpose**: View element at front without removing

**Returns**: Element at front of queue

**Time Complexity**: O(1)

**Example**:
```cpp
Vehicle v = q.peek();  // View front vehicle without removal
```

#### isEmpty()
```cpp
bool Queue<T>::isEmpty()
```
**Purpose**: Check if queue has any elements

**Returns**: true if empty, false otherwise

**Time Complexity**: O(1)

#### size()
```cpp
int Queue<T>::size()
```
**Purpose**: Get number of elements in queue

**Returns**: Current element count

**Time Complexity**: O(1)

### 8.2 Priority Queue Operations

#### insert()
```cpp
void LanePriorityQueue::insert(Lane* lane)
```
**Purpose**: Add lane to priority queue

**Parameters**:
- `lane`: Pointer to Lane object

**Time Complexity**: O(log k) where k = number of lanes

**Algorithm**:
1. Add lane to end of array
2. Increment count
3. Call heapifyUp to restore heap property

#### extractMax()
```cpp
Lane* LanePriorityQueue::extractMax()
```
**Purpose**: Remove and return lane with highest priority

**Returns**: Pointer to Lane with maximum priority

**Time Complexity**: O(log k)

**Algorithm**:
1. Check if empty (throw exception if true)
2. Store root lane (maximum)
3. Move last lane to root
4. Decrement count
5. Call heapifyDown to restore heap property
6. Return stored maximum lane

#### updatePriorities()
```cpp
void LanePriorityQueue::updatePriorities()
```
**Purpose**: Rebuild heap after priority changes

**Time Complexity**: O(k log k)

**Algorithm**:
1. Update priority for each lane
2. Rebuild heap from bottom up using heapifyDown

#### heapifyUp()
```cpp
void LanePriorityQueue::heapifyUp(int index)
```
**Purpose**: Restore heap property upward from given index

**Parameters**:
- `index`: Starting index for heapification

**Time Complexity**: O(log k)

**Algorithm**:
1. If at root, return
2. Calculate parent index: (index-1)/2
3. If parent priority < current priority, swap
4. Recursively call heapifyUp on parent

#### heapifyDown()
```cpp
void LanePriorityQueue::heapifyDown(int index)
```
**Purpose**: Restore heap property downward from given index

**Parameters**:
- `index`: Starting index for heapification

**Time Complexity**: O(log k)

**Algorithm**:
1. Calculate left and right child indices
2. Find largest among current, left child, right child
3. If largest is not current, swap with largest
4. Recursively call heapifyDown on swapped position

### 8.3 Lane Management Functions

#### updatePriority()
```cpp
void Lane::updatePriority()
```
**Purpose**: Update lane priority based on vehicle count

**Algorithm**:
```cpp
if (isPriorityLane && vehicleQueue->size() > 10) {
    priority = 100;  // High priority
} else {
    priority = 0;    // Normal priority
}
```

#### getVehicleCount()
```cpp
int Lane::getVehicleCount()
```
**Purpose**: Get number of waiting vehicles

**Returns**: Vehicle count in queue

**Time Complexity**: O(1)

### 8.4 Simulation Functions

#### loadVehiclesFromFiles()
```cpp
void loadVehiclesFromFiles()
```
**Purpose**: Read vehicle data from lane files and enqueue

**Algorithm**:
1. For each lane file (lanea.txt, etc.)
2. Open file
3. Read line by line
4. Parse CSV format: ID,Time,Lane
5. Create Vehicle object
6. Enqueue to appropriate lane
7. Close and clear file

**Time Complexity**: O(m) where m = vehicles in files

#### processLane()
```cpp
void processLane(Lane* lane)
```
**Purpose**: Process vehicles from a lane

**Parameters**:
- `lane`: Pointer to Lane to process

**Algorithm**:
1. Check if lane has vehicles
2. Calculate vehicles to serve
3. Set traffic light to GREEN
4. For each vehicle to serve:
   - Dequeue vehicle
   - Create animation
   - Increment counter
5. Set traffic light to RED

**Time Complexity**: O(v) where v = vehicles processed

#### calculateVehiclesToServe()
```cpp
int calculateVehiclesToServe(Lane* lane)
```
**Purpose**: Determine how many vehicles to process

**Returns**: Number of vehicles to serve

**Algorithm**: See Section 7.2

#### setTrafficLight()
```cpp
void setTrafficLight(string laneName, LightState state)
```
**Purpose**: Control traffic light state

**Parameters**:
- `laneName`: Lane identifier
- `state`: RED or GREEN

**Algorithm**:
1. If setting to GREEN:
   - Set all other lights to RED
   - Set specified lane to GREEN
2. If setting to RED:
   - Set specified lane to RED

### 8.5 SDL2 Rendering Functions

#### drawRoad()
```cpp
void drawRoad()
```
**Purpose**: Render road system

**Algorithm**:
1. Draw horizontal roads (left and right of junction)
2. Draw vertical roads (top and bottom of junction)
3. Draw junction center
4. Draw lane markings (dashed yellow lines)

#### drawTrafficLight()
```cpp
void drawTrafficLight(int x, int y, LightState state, string label)
```
**Purpose**: Render traffic light

**Parameters**:
- `x, y`: Position coordinates
- `state`: Current light state (RED/GREEN)
- `label
