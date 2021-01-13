#include "core.h"
void createEmpty(queue *Q){
  (*Q).first = NULL;
  (*Q).last = NULL;
  Q->size = 0;
}

int isEmpty(queue Q){
  int hasil = 0;
  if(Q.first == NULL || Q.first == 0x1){
    hasil = 1;
  }
  return hasil;
}

void enqueue(char input, queue *Q ){
  elemen* baru;
  baru = (elemen *) malloc (sizeof (elemen));
  baru->data = input;
  baru->next = NULL;
  if((*Q).first == NULL || Q->first == 0x1){
    (*Q).first = baru;
  }
  else{
    (*Q).last->next = baru;
  }
  (*Q).last = baru;
  Q->size++;
  baru = NULL;
}

char dequeue(queue *Q)
{
    char ans;
    elemen *tmp = (elemen*)malloc(sizeof(elemen));
    if (Q->first == NULL || Q->first == 0x1)
    {
        Q->size = 0;
    }
    else
    {
        tmp = Q->first;
        ans = tmp->data;
        Q->first = Q->first->next;
        Q->size--;
        free(tmp);
    }
    return ans;
}
