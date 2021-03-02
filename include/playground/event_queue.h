#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "event.h"
#include <iostream>

class EventQueue
{
protected:
    /* 큐가 꽉 찬 상태와 텅 빈 상태를 구분하기 위해 하나는 비워둔다 */
    Event** elements;
    int size;
    /* 가장 먼저 들어온 원소의 인덱스 */
    int front;
    /* 다음에 들어올 원소의 자리를 가리키는 인덱스 */
    int rear;

public:
    EventQueue() {}
    EventQueue(int size);
    ~EventQueue();

    bool isEmpty() const;
    bool isFull() const;
    void push(Event*);
    Event* pop();
};

#endif // EVENT_QUEUE_H
