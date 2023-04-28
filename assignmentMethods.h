#pragma once
#include "linkedList.h"

typedef struct Customer {
  int number;
  char service;
} Customer;

typedef struct CustomerArgs {
  LinkedList *list;
  int t_c;
  int m;
} CustomerArgs;

typedef struct Teller{
    LinkedList* list;
    int t_w;
    int t_d;
    int t_i;
}Teller;

void logTofile(char *message);
void printCustomer(void *data);
void *customer(void *data);
int freeCustomer(LinkedList *list);
void *teller(void* data);
