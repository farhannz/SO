#include "core.h"
void createEmpty(queue *Q){
  (*Q).first = NULL;
  (*Q).last = NULL;
  Q->size = 0;
}

int isEmpty(queue Q){
  int hasil = 0;
  if(Q.first == NULL){
    hasil = 1;
  }
  return hasil;
}

void enqueue(char input, queue *Q ){
  elemen* baru;
  baru = (elemen *) malloc (sizeof (elemen));
  baru->data = input;
  baru->next = NULL;
  if((*Q).first == NULL){
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
    if (Q->first == NULL)
    {
        printf("\n\nqueue is empty \n");
    }
    else
    {
        elemen *tmp = Q->first;
        ans = tmp->data;
        Q->first = Q->first->next;
        free(tmp);
    }
    Q->size--;
    return ans;
}