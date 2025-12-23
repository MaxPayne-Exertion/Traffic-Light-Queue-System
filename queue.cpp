#include <iostream>

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
};


class Vehicle {
    public:
    int id;
    int spawntime;

    Vehicle(int id=-1,int s=0):id(id),spawntime(s){}
};

class Vehicle {
    public:
    string id;
    long spawntime;

    Vehicle(string id="-1",long s=0):id(id),spawntime(s){}
    bool operator==(const Vehicle& other) const {
        return id == other.id;}
};

class Lane {
public:
    string name;
    int isPriority;
    Queue<Vehicle> vehicleQ;

    Lane(string name = "", int priority=0)
        : name(name), isPriority(priority) {}
};


class VehicleQueue : public Queue<Vehicle> {};

class LaneQueue : public Queue<Lane> {};




// Priority Queue Implementation
template<typename T>
class PriorityQueue {
private:
    struct PriorityItem {
        T data;
        int priority;
        PriorityItem(T d, int p) : data(d), priority(p) {}
    };

    PriorityItem* items[max];
    int size;

public:
    PriorityQueue() : size(0) {}

    bool isEmpty() { return size == 0; }
    bool isFull() { return size == max; }

    void enqueue(T data, int priority) {
        if (isFull()) {
            cout << "Priority queue is full!" << endl;
            return;
        }

        // Create new item
        items[size] = new PriorityItem(data, priority);

        // Bubble up to maintain heap property
        int i = size;
        while (i > 0 && items[i]->priority > items[(i-1)/2]->priority) {
            swap(items[i], items[(i-1)/2]);
            i = (i-1)/2;
        }
        size++;
    }

    T dequeue() {
        if (isEmpty()) {
            cout << "Priority queue is empty!" << endl;
            return T();
        }

        T result = items[0]->data;
        delete items[0];

        // Move last element to root
        items[0] = items[size-1];
        size--;

        // Heapify down
        int i = 0;
        while (true) {
            int left = 2*i + 1;
            int right = 2*i + 2;
            int largest = i;

            if (left < size && items[left]->priority > items[largest]->priority)
                largest = left;
            if (right < size && items[right]->priority > items[largest]->priority)
                largest = right;

            if (largest == i) break;

            swap(items[i], items[largest]);
            i = largest;
        }

        return result;
    }

    void updatePriority(T data, int newPriority) {
        // Find the item
        for (int i = 0; i < size; i++) {
            if (items[i]->data == data) {
                int oldPriority = items[i]->priority;
                items[i]->priority = newPriority;

                // Adjust position in heap
                if (newPriority > oldPriority) {
                    // Bubble up
                    while (i > 0 && items[i]->priority > items[(i-1)/2]->priority) {
                        swap(items[i], items[(i-1)/2]);
                        i = (i-1)/2;
                    }
                } else {
                    // Heapify down
                    while (true) {
                        int left = 2*i + 1;
                        int right = 2*i + 2;
                        int largest = i;

                        if (left < size && items[left]->priority > items[largest]->priority)
                            largest = left;
                        if (right < size && items[right]->priority > items[largest]->priority)
                            largest = right;

                        if (largest == i) break;

                        swap(items[i], items[largest]);
                        i = largest;
                    }
                }
                return;
            }
        }
    }

    void display() {
        if (isEmpty()) {
            cout << "Priority queue is empty" << endl;
            return;
        }

        cout << "Priority Queue (priority: data): ";
        for (int i = 0; i < size; i++) {
            cout << items[i]->priority << ":" << items[i]->data << " ";
        }
        cout << endl;
    }
};

