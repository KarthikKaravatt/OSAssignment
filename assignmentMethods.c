#include "assignmentMethods.h"
#include "linkedList.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t writeToLog;
pthread_mutex_t listLock;
pthread_mutex_t fileLock;
pthread_cond_t cond;
pthread_cond_t queueFull;
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
  // printf("%s, %c \n", customer->number, customer->service);
}
void logCustomer(char *customerString, char *serviceString,
                 struct tm *timeString) {
  char onlyTime[100];
  sprintf(onlyTime, "%d:%d:%d\n", timeString->tm_hour, timeString->tm_min,
          timeString->tm_sec);
  logTofile("--------------------------------------\n");
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
  pthread_mutex_lock(&writeToLog);
  logCustomer(customerString, serviceString, timeString);
  pthread_mutex_unlock(&writeToLog);
}

void *customer(void *data) {
  CustomerArgs *args = (CustomerArgs *)data;
  LinkedList *list = args->list;
  int t_c = args->t_c;
  int m = args->m;
  FILE *fptr;
  char line[50];
  fptr = fopen("c_file", "r");
  while (fgets(line, sizeof(line), fptr)) {
    printf("customer lock\n");
    pthread_mutex_lock(&listLock);
    addCustomer(list, line, t_c);
    // pthread_cond_signal(&cond);
    sleep(t_c);
    printf("customer added\n");
    if (list->size == m) {
      printf("queFull\n");
      pthread_cond_signal(&cond);
      pthread_cond_wait(&queueFull, &listLock);
    }
    printf("customer unlock\n");
    pthread_mutex_unlock(&listLock);
    pthread_cond_signal(&cond);
  }
  fclose(fptr);
  fileread = 1;
  pthread_cond_signal(&cond);
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
void *teller(void *data) {
  while (fileread == 0) {
    time_t arivalT, responseT;
    struct tm *arivalString, *responseString;
    printf("Teller lock\n");
    pthread_mutex_lock(&listLock);
    Teller *teller = (Teller *)data;
    LinkedList *list = teller->list;
    int m = teller->m;
    printf("%s\n", teller->id);
    if (list->size == 0) {
      printf("teller wait\n");
      pthread_cond_signal(&queueFull);
      pthread_cond_wait(&cond, &listLock);
    }
    Customer *customer = (Customer *)removeFirst(list);
    printf("Teller unlock\n");
    pthread_mutex_unlock(&listLock);
    arivalT = time(&arivalT);
    arivalString = localtime(&arivalT);
    char arivalTime[100];
    sprintf(arivalTime, "%d:%d:%d\n", arivalString->tm_hour,
            arivalString->tm_min, arivalString->tm_sec);
    printf("customer removed\n");
    printf("%s\n", customer->number);
    switch (customer->service) {
    case 'W':
      printf("sleeping\n");
      // pthread_mutex_unlock(&mutex);
      sleep(teller->t_w);
      break;
    case 'D':
      printf("sleeping\n");
      // pthread_mutex_unlock(&mutex);
      sleep(teller->t_d);
      break;
    case 'I':
      printf("sleeping\n");
      // pthread_mutex_unlock(&mutex);
      sleep(teller->t_i);
      break;
    }
    responseT = time(&responseT);
    responseString = localtime(&responseT);
    char responseTime[100];
    sprintf(responseTime, "%d:%d:%d\n", responseString->tm_hour,
            responseString->tm_min, responseString->tm_sec);
    pthread_mutex_lock(&writeToLog);
    logTofile("Teller: ");
    logTofile(teller->id);
    logTofile("\n");
    logTofile("Customer: ");
    logTofile(customer->number);
    logTofile("\n");
    logTofile("Arrival time: ");
    logTofile(arivalTime);
    logTofile("Response time: ");
    logTofile(responseTime);
    pthread_mutex_unlock(&writeToLog);
    if (fileread == 1) {
      break;
    }
  }
  printf("teller terminated\n");
  return EXIT_SUCCESS;
}
