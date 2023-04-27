#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "linkedList.h"
#include "assignmentMethods.h"

void logTofile(char *message){
    FILE *file;
    file = fopen("r_log", "a");
    fprintf(file,"%s", message); 
    fclose(file);
}
void printCustomer(void *data) {
  Customer *customer = (Customer *)data;
  printf("%d, %c \n", customer->number, customer->service);
}
void *customer(void *data) {
  CustomerArgs *args = (CustomerArgs *)data;
  LinkedList *list = args->list;
  int t_c = args->t_c;
  FILE *fptr;
  char line[50];
  logTofile("--------------------------------------\n");
  fptr = fopen("c_file", "r");
  while (fgets(line, sizeof(line), fptr)) {
    time_t curTime;
    struct tm *timeString;
    char* customerString;
    char* serviceString;
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
    logTofile(customerString);
    logTofile(": ");
    logTofile(serviceString);
    logTofile("\n");
    logTofile("Current Time: ");
    logTofile(asctime(timeString));
    logTofile("--------------------------------------\n");
    usleep(t_c);
  }
  fclose(fptr);
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

void teller(Customer *customer){
    sleep(1);
    switch (customer->service) {
        case 'W':
            usleep(20);
            break;
        case 'D':
            usleep(10);
            break;
        case 'I':
            usleep(5);
            break;
    }
}
