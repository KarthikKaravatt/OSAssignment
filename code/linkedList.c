/*author Karthik Karavatt
 * StudentID 20619965 */
/* This linked list will act like a queue*/
// Created when doing a USP practical 
#include "linkedList.h"
#include <stdio.h>
#include <stdlib.h>

LinkedList *createList() {
  LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
  return list;
}

void insertFirst(LinkedList *list, void *data) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->data = data;
  node->next = NULL;
  /* If list is empty */
  if (list->head == NULL) {
    node->next = list->tail;
    list->head = node;
  }
  /* if the list has one item*/
  else if (list->head->next == NULL) {
    Node *temp = list->head;
    temp->before = node;
    list->head = node;
    node->next = temp;
    list->tail = temp;

  } else {
    Node *temp = list->head;
    temp->before = node;
    list->head = node;
    node->next = temp;
  }
  list->size++;
}

void insertLast(LinkedList *list, void *data) {
  Node *currentTail;
  Node *node = (Node *)malloc(sizeof(Node));
  node->data = data;
  node->next = NULL;
  /*if list is empty */
  if (list->head == NULL) {
    list->head = node;
    list->head->next = list->tail;
  } else {
    currentTail = list->tail;
    /*if there is only one node */
    if (currentTail == NULL) {
      list->head->next = node;
      node->before = list->head;
      list->tail = node;
    } else {
      currentTail->next = node;
      node->before = currentTail;
      list->tail = node;
    }
  }
  list->size++;
}
void *removeFirst(LinkedList *list) {
  void *data;
  Node *node;
  /*check if list is empty*/
  if (list->head != NULL) {
    node = list->head;
    data = node->data;
    /*check if there is only one node*/
    if (list->tail == NULL) {
      list->head = NULL;
    }
    /*check if there is only two node*/
    else if (list->head->next == list->tail) {
      list->head = list->tail;
      list->head->before = NULL;
      list->tail = NULL;
    } else {
      data = list->head->data;
      list->head = list->head->next;
      list->head->before = NULL;
    }
    free(node);
    list->size--;
  }
  return data;
}
void *removeLast(LinkedList *list) {
  void *data;
  Node *node;
  /* check if list is empty */
  if (list->head != NULL) {
    /* check if there is more than one node */
    if (list->tail != NULL) {
      node = list->tail;
      data = node->data;
      node->before->next = NULL;
      /* check if there are only two node */
      if (node->before == list->head) {
        list->tail = NULL;
      } else {
        list->tail = node->before;
      }
      node = NULL;
    }
    /* If there is only one node */
    else {
      node = list->head;
      data = node->data;
      list->head = NULL;
    }
    free(node);
    list->size--;
  }
  return data;
}

void printList(LinkedList *list, listFunc func) {
  Node *node = list->head;
  if (node != NULL) {
    while (node != NULL) {
      func(node->data);
      node = node->next;
    }
    node = NULL;
  }
}

void freeNode(LinkedList *list) {
  Node *curNode = list->head;
  Node *nextNode;
  while (curNode != NULL) {
    nextNode = curNode->next;
    free(curNode);
    curNode = nextNode;
    list->size--;
  }
}

void freeList(LinkedList *list) {
  freeNode(list);
  free(list);
}
