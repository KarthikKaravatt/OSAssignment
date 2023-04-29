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
// TODO: Terrible way of implementing this, fix later
int fileread = 0;
void logTofile(char *message) {
  FILE *file;
  file = fopen("r_log", "a");
  fprintf(file, "%s", message);
  fclose(file);
}
void printCustomer(void *data) {
  Customer *customer = (Customer *)data;
  printf("%s, %c \n", customer->number, customer->service);
}
void logCustomer(char *customerString, char *serviceString,
                 struct tm *timeString) {
  char onlyTime[100];
  sprintf(onlyTime, "%d:%d:%d\n", timeString->tm_hour, timeString->tm_min,
          timeString->tm_sec);
  logTofile(customerString);
  logTofile(": ");
  logTofile(serviceString);
  logTofile("\n");
  logTofile("Arival Time: ");
  logTofile(onlyTime);
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
      // customerString[strcspn(customerString, "\n")] = 0;
      customer->number = malloc(strlen(customerString));
      strcpy(customer->number, customerString);
      customer->number = splitString;
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
  pthread_mutex_lock(&mutex);
  while (fgets(line, sizeof(line), fptr)) {
    printf("customer lock\n");
    addCustomer(list, line, t_c);
    sleep(t_c);
    printf("customer added\n");
    while (list->size == m) {
      printf("queFull\n");
      pthread_cond_wait(&cond, &mutex);
    }
    printf("customer unlock\n");
  }
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
  fclose(fptr);
  fileread = 1;
  return EXIT_SUCCESS;
}

int freeCustomer(LinkedList *list) {
  Node *curNode = list->head;
  while (curNode != NULL) {
    Customer *customer = (Customer *)curNode->data;
    curNode = curNode->next;
    free(customer->number);
    free(customer);
  }
  return EXIT_SUCCESS;
}
void logTeller(char *tellerID, char *customerID, char *arivalTime,
               char *responseTime) {
  logTofile("Teller: ");
  logTofile(tellerID);
  logTofile("\n");
  logTofile("Customer: ");
  logTofile(customerID);
  logTofile("\n");
  logTofile("Arrival time: ");
  logTofile("\n");
  logTofile("Response time: ");
  logTofile("\n");
}
void *teller(void *data) {
  time_t arivalTime, responseTime;
  char *arivalString, *responseString;
  time(&arivalTime);
  time(&responseTime);
  // arivalString = localtime(&arivalTime);
  Teller *teller = (Teller *)data;
  LinkedList *list = teller->list;
  int m= teller->m;
  printf("teller lock\n");
  pthread_mutex_lock(&mutex);
  while (list->size == 0) {
    printf("queue empty start\n");
    pthread_cond_wait(&cond, &mutex);
  }
  while (list->size > 0) {
    Customer *customer = (Customer*)removeFirst(list);
    printf("%s\n", customer->number);
    logTeller(&teller->id, customer->number, arivalString, responseString);
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
      printf("queue empty End\n");
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
