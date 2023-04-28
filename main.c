#include "assignmentMethods.h"
#include "linkedList.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  extern pthread_mutex_t mutex; 
pthread_cond_t cond;
  int m = atoi(argv[1]);
  int t_c = atoi(argv[2]);
  int t_w = atoi(argv[3]);
  int t_d = atoi(argv[4]);
  int t_i = atoi(argv[5]);
  pthread_t id, t1, t2, t3, t4;
  CustomerArgs args;
  Teller teller1, teller2, teller3, teller4;
  LinkedList *c_queue = createList();
  teller1.t_i = t_i;
  teller1.t_d = t_d;
  teller1.t_w = t_w;
  teller1.list = c_queue;
  teller2.t_i = t_i;
  teller2.t_d = t_d;
  teller2.t_w = t_w;
  teller2.list = c_queue;
  teller3.t_i = t_i;
  teller3.t_d = t_d;
  teller3.t_w = t_w;
  teller3.list = c_queue;
  teller4.t_i = t_i;
  teller4.t_d = t_d;
  teller4.t_w = t_w;
  teller4.list = c_queue;
  int *ptr;
  void (*listFunc)(void *);
  listFunc = (void *)&printCustomer;
  args.list = c_queue;
  args.t_c = t_c;
  args.m = m;
  pthread_mutex_init(&mutex,NULL); 
  pthread_cond_init(&cond,NULL); 
  pthread_create(&id, NULL, customer, (void *)&args);
  pthread_create(&t1, NULL, teller, (void *)&teller1);
  pthread_join(id,NULL);
  pthread_join(t1,NULL);
  printList(c_queue, listFunc);
  freeCustomer(c_queue);
  freeList(c_queue);
  //TODO: Fix this
  // pthread_mutex_destroy(&mutex);
  return EXIT_SUCCESS;
}
