#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>

template <typename T>
class CircularQueue
{
protected:
    /* 큐가 꽉 찬 상태와 텅 빈 상태를 구분하기 위해 하나는 비워둔다 */
    T* elements;
    int size;
    /* 가장 먼저 들어온 원소의 인덱스 */
    int front;
    /* 다음에 들어올 원소의 자리를 가리키는 인덱스 */
    int rear;

public:
    CircularQueue() {}
    CircularQueue(int size);
    ~CircularQueue();

    bool isEmpty() const;
    bool isFull() const;
    void push(T);
    T pop();
};

template <typename T>
CircularQueue<T>::CircularQueue(int _size)
{
    elements = new T[_size];
    size = _size;
    front = rear = 0;
}

template <typename T>
CircularQueue<T>::~CircularQueue()
{
    delete[] elements;
}

template <typename T>
bool CircularQueue<T>::isEmpty() const
{
    if (front == rear)
        return true;
    else
        return false;
}

template <typename T>
bool CircularQueue<T>::isFull() const
{
    if (front == (rear+1) % size)
        return true;
    else
        return false;
}

template <typename T>
void CircularQueue<T>::push(T newElement)
{
    if (isFull())
    {
        std::cout << "CircularQueue::push()::Queue is already full" << std::endl;
        return;
    }

    elements[rear] = newElement;
    rear = (rear+1) % size;
}

template <typename T>
T CircularQueue<T>::pop()
{
    if (isEmpty())
    {
        std::cout << "CircularQueue::pop()::Queue is already empty" << std::endl;
        return T();
    }

    T result = elements[front];
    front = (front+1) % size;

    return result;
}

#endif // QUEUE_H
