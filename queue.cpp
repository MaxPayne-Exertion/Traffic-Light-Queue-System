#include <iostream>

using namespace std;
#define max 20
class Queue{
    public:
    int front;
    int rear;
    int arr[max];

    Queue():front(-1),rear(-1){}

    bool isEmpty(){
        return front==-1||front>rear;
    }
    bool isFull(){
        return rear==max-1;
    }

    int getFront(){
        if(isEmpty()){
            cout<<"Empty"<<endl;
            return -1;
        }
        return arr[front];
    }
    int getRear(){
        if(isEmpty()){
            cout<<"Empty"<<endl;
            return -1;
        }
        return arr[rear];
    }


    void enqueue(int val){
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
    int dequeue()
    {
        if(isEmpty()){
            cout<<"empty"<<endl;
            return -1;
        }
        int data=arr[front];
        front++;
        if (isEmpty()){
            front=rear=-1;
        }
        return data;
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


int main()
{
    // Created Queue of size 5
    Queue q;

    // Enqueueing elements
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);

    // Displaying status of the queue after enqueuing
    cout << "\nAfter Enqueueing:" << endl;

    cout << "Front element: " << q.getFront() << endl;
    cout << "Rear element: " << q.getRear() << endl;

    q.display();

    // Enqueueing more elements
    q.enqueue(4);
    q.enqueue(5);

    // Displaying the updated queue
    q.display();

    // Enqueueing one more element to demonstrate overflow
    // condition
    q.enqueue(6);

    // Dequeueing elements
    cout << "\nDequeueing elements:" << endl;
    cout << "Dequeued element: " << q.dequeue() << endl;
    cout << "Dequeued element: " << q.dequeue() << endl;

    // Displaying status of the queue after dequeueing
    cout << "\nAfter Dequeueing:" << endl;

    cout << "Front element: " << q.getFront() << endl;
    cout << "Rear element: " << q.getRear() << endl;

    q.display();

    return 0;
}
