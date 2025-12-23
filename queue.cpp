#include <iostream>
#include <string>
#include <vector>
using namespace std;
#define max 20

template<typename T>
class Queue{
    public:
    int front, rear;
    T arr[max];

    Queue():front(-1),rear(-1){}

    bool isEmpty(){
        return front==-1||front>rear;
    }
    bool isFull(){
        return rear==max-1;
    }

    T getFront(){
        if(isEmpty()){
            cout<<"Empty"<<endl;
            return T();
        }
        return arr[front];
    }
    T getRear(){
        if(isEmpty()){
            cout<<"Empty"<<endl;
            return T();
        }
        return arr[rear];
    }


    void enqueue(T val){
        if (isFull()){
            cout<<"full"<<endl;
            return;
        }
        if (isEmpty()){
            front=0;
        }
        rear++;
        arr[rear]=val;

    }
    T dequeue()
    {
        if(isEmpty()){
            cout<<"empty"<<endl;
            return T();
        }
        T data=arr[front];
        front++;
        if (isEmpty()){
            front=rear=-1;
        }
        return data;
    }


    int size() {
        if (isEmpty()) {
            return 0;
        }
        return rear - front + 1;
    }


    void display()
    {
        if (isEmpty()) {
            cout << "Queue is empty" << endl;
            return;
        }
        cout << "Queue:  ";
        for (int i = front; i <= rear; i++) {
            cout << arr[i] << " ";
        }

        cout << endl;
    }
    void clear() {
        front = rear = -1;
    }

    bool contains(T value) {
        for (int i = front; i <= rear; i++) {
            if (arr[i] == value) return true;
        }}

};
// Priority Queue for lanes
class PriorityQueue {
private:
    struct LanePriority {
        int laneIndex;
        int priority;
        LanePriority(int idx, int p) : laneIndex(idx), priority(p) {}
    };

    vector<LanePriority> heap;

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
        heap.push_back(LanePriority(laneIndex, priority));
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
        // Find the lane
        for (int i = 0; i < heap.size(); i++) {
            if (heap[i].laneIndex == laneIndex) {
                int oldPriority = heap[i].priority;
                heap[i].priority = newPriority;

                if (newPriority > oldPriority) heapifyUp(i);
                else if (newPriority < oldPriority) heapifyDown(i);
                return;
            }
        }
        // If not found, insert it
        insert(laneIndex, newPriority);
    }

    int getPriority(int laneIndex) {
        for (auto& item : heap) {
            if (item.laneIndex == laneIndex) return item.priority;
        }
        return 0;
    }

    bool isEmpty() { return heap.empty(); }
};

// Vehicle class
class Vehicle {
public:
    string id;
    long timestamp;

    Vehicle(string id = "", long ts = 0) : id(id), timestamp(ts) {}
};

// Lane class
class Lane {
public:
    string name;
    Queue<Vehicle> vehicles;

    Lane(string n = "") : name(n) {}
};
