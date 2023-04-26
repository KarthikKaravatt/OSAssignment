#include "linkedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Customer {
  int number;
  char service;
} Customer;
void printCustomer(void *data) {
  Customer *customer = (Customer *)data;
  printf("%d, %c \n", customer->number, customer->service);
}
int customer(LinkedList *list, int t_c) {
  FILE *fptr;
  char line[50];
  fptr = fopen("c_file", "r");
  while (fgets(line, sizeof(line), fptr)) {
    char *splitString;
    Customer *customer = malloc(sizeof(Customer));
    int index = 0;
    splitString = strtok(line, " ");
    while (splitString != NULL) {
      if (index == 0) {
        customer->number = atoi(splitString);
      } else if (index == 1) {
        customer->service = *splitString;
      }
      index++;
      splitString = strtok(NULL, " ");
    }
    insertLast(list, (void *)customer);
    sleep(t_c);
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
int main(int argc, char *argv[]) {
  void (*listFunc)(void *);
  listFunc = (void *)&printCustomer;
  LinkedList *c_queue = createList();
  customer(c_queue, 1);
  printList(c_queue, listFunc);
  freeCustomer(c_queue);
  freeList(c_queue);
  return EXIT_SUCCESS;
}
