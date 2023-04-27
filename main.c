#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "assignmentMethods.h"
#include "linkedList.h"
int main(int argc, char *argv[]) {
  int m = atoi(argv[1]);
  int t_c = atoi(argv[2]);
  int t_w = atoi(argv[3]);
  int t_d = atoi(argv[4]);
  int t_i = atoi(argv[5]);
  pthread_t id;
  CustomerArgs args;
  void (*listFunc)(void *);
  int *ptr;
  listFunc = (void *)&printCustomer;
  LinkedList *c_queue = createList();
  args.list = c_queue;
  args.t_c = t_c;
  pthread_create(&id, NULL, customer, (void *)&args);
  pthread_join(id, (void **)&ptr);
  printList(c_queue, listFunc);
  freeCustomer(c_queue);
  freeList(c_queue);
  return EXIT_SUCCESS;
}
