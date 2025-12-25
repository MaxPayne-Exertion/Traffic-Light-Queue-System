#include <iostream>
#include <string>
#include <vector>

using namespace std;
#define MAX_SIZE 50

template<typename T>
class Queue {
public:
    int front, rear;
    T arr[MAX_SIZE];

    Queue() : front(-1), rear(-1) {}

    bool isEmpty() { return front == -1 || front > rear; }
    bool isFull() { return rear == MAX_SIZE - 1; }

    void enqueue(T val) {
        if (isFull()) return;
        if (isEmpty()) front = 0;
        rear++;
        arr[rear] = val;
    }

    T dequeue() {
        if (isEmpty()) return T();
        T data = arr[front];
        front++;
        if (isEmpty()) front = rear = -1;
        return data;
    }

    int size() {
        if (isEmpty()) return 0;
        return rear - front + 1;
    }
};

class PriorityQueue {
private:
    struct Item {
        int laneIndex;
        int priority;
        Item(int idx, int p) : laneIndex(idx), priority(p) {}
    };

    vector<Item> heap;

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index].priority > heap[parent].priority) {
                swap(heap[index], heap[parent]);
                index = parent;
            } else break;
        }
    }

    void heapifyDown(int index) {
        int size = heap.size();
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < size && heap[left].priority > heap[largest].priority)
                largest = left;
            if (right < size && heap[right].priority > heap[largest].priority)
                largest = right;

            if (largest != index) {
                swap(heap[index], heap[largest]);
                index = largest;
            } else break;
        }
    }

public:
    PriorityQueue() {}

    void insert(int laneIndex, int priority) {
        heap.push_back(Item(laneIndex, priority));
        heapifyUp(heap.size() - 1);
    }

    int extractMax() {
        if (heap.empty()) return -1;

        int maxLane = heap[0].laneIndex;
        heap[0] = heap.back();
        heap.pop_back();

        if (!heap.empty()) heapifyDown(0);
        return maxLane;
    }

    void updatePriority(int laneIndex, int newPriority) {
        for (int i = 0; i < heap.size(); i++) {
            if (heap[i].laneIndex == laneIndex) {
                int oldPriority = heap[i].priority;
                heap[i].priority = newPriority;

                if (newPriority > oldPriority) heapifyUp(i);
                else if (newPriority < oldPriority) heapifyDown(i);
                return;
            }
        }
        insert(laneIndex, newPriority);
    }

    int getPriority(int laneIndex) {
        for (auto& item : heap) {
            if (item.laneIndex == laneIndex) return item.priority;
        }
        return 0;
    }
};

class Vehicle {
public:
    string id;
    long timestamp;

    Vehicle(string id = "", long ts = 0) : id(id), timestamp(ts) {}
};

class Lane {
public:
    string name;
    Queue<Vehicle> vehicles;

    Lane(string n = "") : name(n) {}
};
