#include "assignmentMethods.h"
#include "linkedList.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t mutex;
pthread_cond_t cond;
//TODO: Terrible way of implementing this, fix later
int fileread = 0;
void logTofile(char *message) {
  FILE *file;
  file = fopen("r_log", "a");
  fprintf(file, "%s", message);
  fclose(file);
}
void printCustomer(void *data) {
  Customer *customer = (Customer *)data;
  printf("%d, %c \n", customer->number, customer->service);
}
void logCustomer(char *customerString, char *serviceString,
                 struct tm *timeString) {
  logTofile(customerString);
  logTofile(": ");
  logTofile(serviceString);
  logTofile("\n");
  logTofile("Current Time: ");
  logTofile(asctime(timeString));
  logTofile("--------------------------------------\n");
}
void addCustomer(LinkedList *list, char line[], int t_c) {
  time_t curTime;
  struct tm *timeString;
  char *customerString;
  char *serviceString;
  char *splitString;
  Customer *customer = malloc(sizeof(Customer));
  int index = 0;
  splitString = strtok(line, " \n\0");
  while (splitString != NULL) {
    if (index == 0) {
      customerString = splitString;
      customerString[strcspn(customerString, "\n")] = 0;
      customer->number = atoi(splitString);
    } else if (index == 1) {
      serviceString = splitString;
      serviceString[strcspn(serviceString, "\n")] = 0;
      customer->service = *splitString;
    }
    index++;
    splitString = strtok(NULL, " ");
  }
  time(&curTime);
  timeString = localtime(&curTime);
  insertLast(list, (void *)customer);
  logCustomer(customerString, serviceString, timeString);
}

void *customer(void *data) {
  CustomerArgs *args = (CustomerArgs *)data;
  LinkedList *list = args->list;
  int t_c = args->t_c;
  int m = args->m;
  FILE *fptr;
  char line[50];
  logTofile("--------------------------------------\n");
  fptr = fopen("c_file", "r");
  while (fgets(line, sizeof(line), fptr)) {
    printf("customer lock\n");
    pthread_mutex_lock(&mutex);
    addCustomer(list, line, t_c);
    pthread_cond_signal(&cond);
    printf("customer added\n");
    while (list->size == 2) {
      printf("queFull\n");
      pthread_cond_wait(&cond, &mutex);
    }
    sleep(t_c);
    printf("customer unlock\n");
    pthread_mutex_unlock(&mutex);
  }
  pthread_cond_signal(&cond);
  fclose(fptr);
  fileread = 1;
  return EXIT_SUCCESS;
}

int freeCustomer(LinkedList *list) {
  Node *curNode = list->head;
  while (curNode != NULL) {
    Customer *customer = (Customer *)curNode->data;
    curNode = curNode->next;
    free(customer);
  }
  return EXIT_SUCCESS;
}
void *teller(void *data) {
  Teller *teller = (Teller *)data;
  LinkedList *list = teller->list;
  printf("teller lock\n");
  pthread_mutex_lock(&mutex);
  while (list->size == 0) {
    printf("queue empty\n");
    pthread_cond_wait(&cond, &mutex);
  }
  while (list->size > 0) {
    Customer *customer = (Customer *)removeFirst(list);
    pthread_cond_signal(&cond);
    printf("customer removed\n");
    switch (customer->service) {
    case 'W':
      sleep(teller->t_w);
      break;
    case 'D':
      sleep(teller->t_d);
      break;
    case 'I':
      sleep(teller->t_i);
      break;
    }
    while (list->size == 0) {
      printf("queue empty\n");
      if (fileread == 1) {
        break;
      }
      pthread_cond_wait(&cond, &mutex);
    }
  }
  printf("teller unlock\n");
  pthread_mutex_unlock(&mutex);
  return EXIT_SUCCESS;
}
