#include <playground/event_queue.h>

EventQueue::EventQueue(int _size)
{
    size = _size;
    elements = new Event*[_size];
    front = rear = 0;
}

EventQueue::~EventQueue()
{
    while (!isEmpty())
        delete pop();
    delete[] elements;
}

bool EventQueue::isEmpty() const
{
    if (front == rear)
        return true;
    else
        return false;
}

bool EventQueue::isFull() const
{
    if (front == (rear+1) % size)
        return true;
    else
        return false;
}

void EventQueue::push(Event* newElement)
{
    if (isFull())
    {
        std::cout << "EventQueue::push()::Queue is already full" << std::endl;
        return;
    }

    elements[rear] = newElement;
    rear = (rear+1) % size;
}

Event* EventQueue::pop()
{
    if (isEmpty())
    {
        std::cout << "EventQueue::pop()::Queue is already empty" << std::endl;
        return nullptr;
    }

    Event* result = elements[front];
    front = (front+1) % size;

    return result;
}
