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
