#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <string>
#include <ctime>

// Vehicle class to represent a vehicle
class Vehicle {
public:
    std::string id;
    time_t arrivalTime;
    std::string lane;
    
    Vehicle() : id(""), arrivalTime(0), lane("") {}
    Vehicle(std::string vid, time_t aTime, std::string vLane) 
        : id(vid), arrivalTime(aTime), lane(vLane) {}
};

// Node for Queue implementation
template <typename T>
class QNode {
public:
    T data;
    QNode* next;
    
    QNode(T d) : data(d), next(nullptr) {}
};

// Queue class
template <typename T>
class Queue {
private:
    QNode<T>* front;
    QNode<T>* rear;
    int count;
    
public:
    Queue() : front(nullptr), rear(nullptr), count(0) {}
    
    ~Queue() {
        while (!isEmpty()) {
            dequeue();
        }
    }
    
    void enqueue(T data) {
        QNode<T>* temp = new QNode<T>(data);
        
        if (rear == nullptr) {
            front = rear = temp;
        } else {
            rear->next = temp;
            rear = temp;
        }
        count++;
    }
    
    T dequeue() {
        if (isEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        QNode<T>* temp = front;
        T data = front->data;
        front = front->next;
        
        if (front == nullptr) {
            rear = nullptr;
        }
        
        delete temp;
        count--;
        return data;
    }
    
    T peek() {
        if (isEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        return front->data;
    }
    
    bool isEmpty() {
        return front == nullptr;
    }
    
    int size() {
        return count;
    }
    
    void display() {
        QNode<T>* temp = front;
        while (temp != nullptr) {
            std::cout << temp->data.id << " ";
            temp = temp->next;
        }
        std::cout << std::endl;
    }
};

// Lane class to represent a lane with priority
class Lane {
public:
    std::string name;
    Queue<Vehicle>* vehicleQueue;
    int priority;
    bool isPriorityLane;
    
    Lane() : name(""), vehicleQueue(new Queue<Vehicle>()), priority(0), isPriorityLane(false) {}
    
    Lane(std::string n, bool isPriority = false) 
        : name(n), vehicleQueue(new Queue<Vehicle>()), priority(0), isPriorityLane(isPriority) {}
    
    ~Lane() {
        delete vehicleQueue;
    }
    
    int getVehicleCount() {
        return vehicleQueue->size();
    }
    
    void updatePriority() {
        if (isPriorityLane && vehicleQueue->size() > 10) {
            priority = 100; // High priority
        } else {
            priority = 0; // Normal priority
        }
    }
};

// Priority Queue for Lanes
class LanePriorityQueue {
private:
    Lane** lanes;
    int capacity;
    int count;
    
    void heapifyUp(int index) {
        if (index == 0) return;
        
        int parent = (index - 1) / 2;
        if (lanes[parent]->priority < lanes[index]->priority) {
            Lane* temp = lanes[parent];
            lanes[parent] = lanes[index];
            lanes[index] = temp;
            heapifyUp(parent);
        }
    }
    
    void heapifyDown(int index) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;
        
        if (left < count && lanes[left]->priority > lanes[largest]->priority) {
            largest = left;
        }
        
        if (right < count && lanes[right]->priority > lanes[largest]->priority) {
            largest = right;
        }
        
        if (largest != index) {
            Lane* temp = lanes[index];
            lanes[index] = lanes[largest];
            lanes[largest] = temp;
            heapifyDown(largest);
        }
    }
    
public:
    LanePriorityQueue(int cap = 10) : capacity(cap), count(0) {
        lanes = new Lane*[capacity];
    }
    
    ~LanePriorityQueue() {
        delete[] lanes;
    }
    
    void insert(Lane* lane) {
        if (count >= capacity) {
            throw std::runtime_error("Priority queue is full");
        }
        
        lanes[count] = lane;
        heapifyUp(count);
        count++;
    }
    
    Lane* extractMax() {
        if (count == 0) {
            throw std::runtime_error("Priority queue is empty");
        }
        
        Lane* maxLane = lanes[0];
        lanes[0] = lanes[count - 1];
        count--;
        heapifyDown(0);
        
        return maxLane;
    }
    
    void updatePriorities() {
        for (int i = 0; i < count; i++) {
            lanes[i]->updatePriority();
        }
        
        // Rebuild heap
        for (int i = count / 2 - 1; i >= 0; i--) {
            heapifyDown(i);
        }
    }
    
    bool isEmpty() {
        return count == 0;
    }
    
    int size() {
        return count;
    }
    
    Lane* peek() {
        if (count == 0) {
            throw std::runtime_error("Priority queue is empty");
        }
        return lanes[0];
    }
};

#endif