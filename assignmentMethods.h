#pragma once
#include "linkedList.h"

typedef struct Customer {
  int number;
  char service;
} Customer;
typedef struct CustomerArgs {
  LinkedList *list;
  int t_c;
} CustomerArgs;
void logTofile(char *message);
void printCustomer(void *data);
void *customer(void *data);
int freeCustomer(LinkedList *list);

