//Queue.h; // interface to Queue ADT
// Written by John Shepherd, March 2013
// Queue ADT code adapted from lab05 of COMP2521 20T2

#ifndef QUEUE_H
#define QUEUE_H


typedef struct QueueRep *Queue;
typedef int Item;

Queue newQueue(); // create new empty queue
void dropQueue(Queue); // free memory used by queue
void showQueue(Queue); // display as 3 > 5 > 4 > ...
void QueueJoin(Queue,Item); // add item on queue
Item QueueLeave(Queue); // remove item from queue
int QueueIsEmpty(Queue); // check for no items
int QueueSize(Queue Q); // get size of queue

#endif
