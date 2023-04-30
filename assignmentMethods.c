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
pthread_cond_t continueOperation;
int fileread = 0;
int served[4];
int tellersLeft = 4;
void logTofile(char *message) {
  FILE *file;
  file = fopen("r_log", "a");
  fprintf(file, "%s", message);
  fflush(file);
  fclose(file);
}
void printCustomer(void *data) {
  Customer *customer = (Customer *)data;
  // printf("%s, %c \n", customer->number, customer->service);
}
void logCustomer(char *customerString, char *serviceString, char *onlyTime) {
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
  char *onlyTime = (char *)malloc((101) * sizeof(char));
  sprintf(onlyTime, "%d:%d:%d\n", timeString->tm_hour, timeString->tm_min,
          timeString->tm_sec);
  customer->arivalTime = onlyTime;
  insertLast(list, (void *)customer);
  pthread_mutex_lock(&writeToLog);
  logCustomer(customerString, serviceString, onlyTime);
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
    if (list->size == m) {
      printf("queFull\n");
      pthread_cond_signal(&cond);
      pthread_cond_wait(&queueFull, &listLock);
    }
    printf("customer added\n");
    addCustomer(list, line, t_c);
    printf("%d\n", list->size);
    // pthread_cond_signal(&cond);
    pthread_mutex_unlock(&listLock);
    sleep(t_c);
    pthread_mutex_lock(&listLock);
    pthread_mutex_unlock(&listLock);
    printf("customer unlock\n");
    pthread_cond_signal(&cond);
  }
  fclose(fptr);
  pthread_mutex_lock(&listLock);
  while (list->size != 0) {
    printf("queue is not empty\n");
    pthread_cond_signal(&cond);
    pthread_cond_wait(&queueFull, &listLock);
  }
  pthread_mutex_unlock(&listLock);
  pthread_mutex_lock(&fileLock);
  fileread = 1;
  pthread_mutex_unlock(&fileLock);
  pthread_cond_broadcast(&cond);
  printf("customer terminated\n");
  return EXIT_SUCCESS;
}

int freeCustomer(LinkedList *list) {
  Node *curNode = list->head;
  while (curNode != NULL) {
    Customer *customer = (Customer *)curNode->data;
    curNode = curNode->next;
    free(customer->arivalTime);
    free(customer->number);
    free(customer);
  }
  return EXIT_SUCCESS;
}
void *teller(void *data) {
  pthread_mutex_lock(&fileLock);
  time_t completeionT, responseT;
  struct tm *completeionString, *responseString;
  printf("Teller lock\n");
  Teller *teller = (Teller *)data;
  LinkedList *list = teller->list;
  int m = teller->m;
  while (fileread == 0) {
    pthread_mutex_unlock(&fileLock);
    pthread_mutex_lock(&listLock);
    if (list->size == 0) {
      printf("teller wait\n");
      pthread_cond_signal(&queueFull);
      pthread_cond_wait(&cond, &listLock);
    } else {
      Customer *customer = (Customer *)removeFirst(list);
      teller->served += 1;
      served[atoi(teller->id) - 1] = teller->served;
      pthread_mutex_unlock(&listLock);
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
      logTofile(customer->arivalTime);
      logTofile("Response time: ");
      logTofile(responseTime);
      pthread_mutex_unlock(&writeToLog);
      printf("Teller unlock\n");
      printf("customer removed\n");
      switch (customer->service) {
      case 'W':
        printf("sleeping\n");
        sleep(teller->t_w);
        break;
      case 'D':
        printf("sleeping\n");
        sleep(teller->t_d);
        break;
      case 'I':
        printf("sleeping\n");
        sleep(teller->t_i);
        break;
      }
      char completeionTime[100];
      completeionT = time(&responseT);
      completeionString = localtime(&responseT);
      sprintf(completeionTime, "%d:%d:%d\n", completeionString->tm_hour,
              completeionString->tm_min, completeionString->tm_sec);
      pthread_mutex_lock(&writeToLog);
      logTofile("Teller: ");
      logTofile(teller->id);
      logTofile("\n");
      logTofile("Customer: ");
      logTofile(customer->number);
      logTofile("\n");
      logTofile("Arrival time: ");
      logTofile(customer->arivalTime);
      logTofile("Completion time: ");
      logTofile(completeionTime);
      pthread_mutex_unlock(&writeToLog);
      pthread_mutex_lock(&fileLock);
      pthread_mutex_lock(&listLock);
    }
    printf("%d\n", tellersLeft);
    if (tellersLeft == 1) {
      char served1[100];
      char served2[100];
      char served3[100];
      char served4[100];
      printf("last teller");
      pthread_mutex_lock(&writeToLog);
      logTofile("Teller 1 serverd: ");
      sprintf(served1, "%d\n", served[0]);
      logTofile(served1);
      logTofile("Teller 2 serverd: ");
      sprintf(served2, "%d\n", served[1]);
      logTofile(served2);
      logTofile("Teller 3 serverd: ");
      sprintf(served3, "%d\n", served[2]);
      logTofile(served3);
      logTofile("Teller 4 serverd: ");
      sprintf(served4, "%d\n", served[3]);
      logTofile(served4);
      pthread_mutex_unlock(&writeToLog);
    }
    pthread_mutex_unlock(&listLock);
  }
  tellersLeft--;
  printf("%d served by %s\n", teller->served, teller->id);
  pthread_mutex_unlock(&listLock);
  pthread_mutex_unlock(&fileLock);
  printf("teller terminated\n");
  return EXIT_SUCCESS;
}
