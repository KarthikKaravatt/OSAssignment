//Name: Karthik Karavatt
//StudentID: 20619965
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

// Globals
pthread_mutex_t writeToLog;
pthread_mutex_t listLock;
pthread_mutex_t fileLock;
pthread_cond_t cond;
pthread_cond_t queueFull;
int fileread = 0; // Indicates if all customers have been dealt with
int served[4]; // array holding info about how many cusomers a teller has served
int tellersLeft = 4; // how many tellers are still running (not terminated)
// logs a string to the log file
void logTofile(char *message) {
  FILE *file;
  file = fopen("r_log", "a");
  fprintf(file, "%s", message);
  fflush(file);
  fclose(file);
}
// log some common infor about the customer
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
// add customer from the file to the queue
void addCustomer(LinkedList *list, char line[], int t_c) {
  time_t curTime;
  struct tm *timeString;
  char *customerString;
  char *serviceString;
  char *splitString;
  Customer *customer = malloc(sizeof(Customer));
  int index = 0;
  // split a line from the file into sub strings diliter is the " "
  splitString = strtok(line, " \n\0");
  // itterate through split string
  while (splitString != NULL) {
    if (index == 0) {
      customerString = splitString;
      customer->number = (char *)malloc(strlen(customerString) + 1);
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
  // lock the opertion to write to the file
  pthread_mutex_lock(&writeToLog);
  logCustomer(customerString, serviceString, onlyTime);
  // unlock the opertion to write to the file
  pthread_mutex_unlock(&writeToLog);
}

// customer function
void *customer(void *data) {
  CustomerArgs *args = (CustomerArgs *)data;
  LinkedList *list = args->list;
  int t_c = args->t_c;
  int m = args->m;
  FILE *fptr;
  char line[50];
  fptr = fopen("c_file", "r");
  // itterates through the whole file
  while (fgets(line, sizeof(line), fptr)) {
    // lock the access to the linked list
    pthread_mutex_lock(&listLock);
    if (list->size == m) {
      // wiatis until the queue is empty before adding more customers
      pthread_cond_wait(&queueFull, &listLock);
    }
    addCustomer(list, line, t_c);
    //signal that customer has been added
    pthread_cond_signal(&cond);
    // unlock the access to the queue
    pthread_mutex_unlock(&listLock);
    // sleeps
    sleep(t_c);
    // locks the list again
  }
  fclose(fptr);
  pthread_mutex_lock(&listLock);
  // if the whole file has been read, customer will signal the teller until
  // queue is empty this is so there is no dead lock between the tellers Because
  // they are waiting for the signal from the customer
  while (list->size != 0) {
    pthread_cond_signal(&cond);
    pthread_cond_wait(&queueFull, &listLock);
  }
  pthread_mutex_unlock(&listLock);
  // changing the file read value indicates to tellers they shoud terminate
  pthread_mutex_lock(&fileLock);
  fileread = 1;
  pthread_mutex_unlock(&fileLock);
  // broadcast signals all tellers that they should terminate
  pthread_cond_broadcast(&cond);
  return EXIT_SUCCESS;
}

// free all customers left in queue
void *teller(void *data) {
  time_t completeionT, responseT;
  struct tm *completeionString, *responseString;
  Teller *teller = (Teller *)data;
  LinkedList *list = teller->list;
  int m = teller->m;
  pthread_mutex_lock(&fileLock);
  while (fileread == 0) {
    pthread_mutex_unlock(&fileLock);
    pthread_mutex_lock(&listLock);
    if (list->size == 0) {
      // wait for a signal from the customer when the queue is full
      pthread_cond_wait(&cond, &listLock);
    } else {
      Customer *customer = (Customer *)removeFirst(list);
      // signal to the customer function that a customer has been removed  
      pthread_cond_signal(&queueFull);
      pthread_mutex_unlock(&listLock);
      teller->served += 1;
      served[atoi(teller->id) - 1] = teller->served;
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
      // unlocks when customer is being servieced
      pthread_mutex_unlock(&writeToLog);
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
      // Acessing fileread vairable is locked now
      pthread_mutex_lock(&fileLock);
      // locks it again 
      pthread_mutex_lock(&listLock);
      free(customer->arivalTime);
      free(customer->number);
      free(customer);
    }
    // check if the teller is the last one left
    // list lock is still enabled here so checking tellers left is okay
    if (tellersLeft == 1) {
      char served1[100];
      char served2[100];
      char served3[100];
      char served4[100];
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
  // decremetns this value when a teller terminates 
  tellersLeft--;
  pthread_mutex_unlock(&listLock);
  pthread_mutex_unlock(&fileLock);
  return EXIT_SUCCESS;
}
