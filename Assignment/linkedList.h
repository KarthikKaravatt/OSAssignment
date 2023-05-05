#pragma once
typedef struct Node
{
   void* data; 
   struct Node* before;
   struct Node* next;
}Node;
typedef struct LinkedList
{
   Node* head;
   Node* tail;
   int size;
}LinkedList;
typedef void(*listFunc)(void* data);
LinkedList* createList(void);
void insertFirst(LinkedList* list, void* data);
void insertLast(LinkedList* list, void* data);
void* removeFirst(LinkedList* list);
void* removeLast(LinkedList* list);
void printList(LinkedList* list, listFunc func);
void freeList(LinkedList* list);
