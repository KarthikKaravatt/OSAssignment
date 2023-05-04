//Name: Karthik Karavatt
//StudentID: 20619965
#include "assignmentMethods.h"
#include "linkedList.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  //global variable from assignmentMethods.c
  extern pthread_mutex_t writeToLog;
  extern pthread_mutex_t listLock;
  extern pthread_mutex_t fileLock;
  extern pthread_cond_t cond;
  extern pthread_cond_t queueFull;
  //command line arguments
  int m = atoi(argv[1]);
  int t_c = atoi(argv[2]);
  int t_w = atoi(argv[3]);
  int t_d = atoi(argv[4]);
  int t_i = atoi(argv[5]);
  // M has to be greater than 0 for the program to work
  // Otherwise it will not make sense 
  if(m <= 0){
      exit(0);
  }
  pthread_t id, t1, t2, t3, t4;
  CustomerArgs args;
  Teller teller1, teller2, teller3, teller4;
  //using linked list made in USP to act as a queueFull
  //The only methods that will be used is insert last and remove first
  //Therefore it is equivilent to a queue
  LinkedList *c_queue = createList();
  //assigning tellers variables
  teller1.id = "1";
  teller1.t_i = t_i;
  teller1.m = m;
  teller1.t_d = t_d;
  teller1.t_w = t_w;
  teller1.list = c_queue;
  teller1.served = 0;
  teller2.id = "2";
  teller2.m = m;
  teller2.t_i = t_i;
  teller2.t_d = t_d;
  teller2.t_w = t_w;
  teller2.list = c_queue;
  teller2.served = 0;
  teller3.id = "3";
  teller3.m = m;
  teller3.t_i = t_i;
  teller3.t_d = t_d;
  teller3.t_w = t_w;
  teller3.list = c_queue;
  teller3.served = 0;
  teller4.id = "4";
  teller4.m = m;
  teller4.t_i = t_i;
  teller4.t_d = t_d;
  teller4.t_w = t_w;
  teller4.list = c_queue;
  teller4.served = 0;
  //arguments for the customer thread
  args.list = c_queue;
  args.t_c = t_c;
  args.m = m;
  // thread initalization
  pthread_mutex_init(&listLock, NULL);
  pthread_mutex_init(&writeToLog, NULL);
  pthread_mutex_init(&fileLock, NULL);
  pthread_cond_init(&cond, NULL);
  pthread_cond_init(&queueFull, NULL);
  //thread creation
  pthread_create(&id, NULL, customer, (void *)&args);
  pthread_create(&t1, NULL, teller, (void *)&teller1);
  pthread_create(&t2, NULL, teller, (void *)&teller2);
  pthread_create(&t3, NULL, teller, (void *)&teller3);
  pthread_create(&t4, NULL, teller, (void *)&teller4);
  //thread join
  pthread_join(id, NULL);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);
  // freeing resources
  freeList(c_queue);
  pthread_mutex_destroy(&listLock);
  pthread_mutex_destroy(&writeToLog);
  pthread_cond_destroy(&cond);
  pthread_cond_destroy(&queueFull);
  return EXIT_SUCCESS;
}
