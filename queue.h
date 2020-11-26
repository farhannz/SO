
#ifndef QUEUE_H
#define QUEUE_H

#include "core.h"
typedef struct elm{
  char data;
  struct elm *next;
}elemen;
typedef struct{
  elemen *first;
  elemen *last;
}queue;

void createEmpty(queue *Q);
int isEmpty(queue Q);
int countElement(queue Q);
void enqueue(char, queue *Q);
char dequeue(queue *Q);


#endif // !QUEUE_H
