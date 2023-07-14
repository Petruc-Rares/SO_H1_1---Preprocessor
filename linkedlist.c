#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "linkedlist.h"

int getSize(LinkedList *list) {
    LinkedListNode *crt_node;
    int size = 0;

    if (list == NULL) return 0;

    crt_node = list->head;

    while (crt_node != NULL) {
        size++;
        crt_node = crt_node->next;
    }

    return size;
}

void insertNode(LinkedList **list, LinkedListNode *node) {
	LinkedListNode *crt_node;
	LinkedListNode *prev;

    if (*list == NULL) {
        printf("list was not allocated\n");
        return;
    }

    crt_node = (*list)->head;


    // list is empty
    if (crt_node == NULL) {
        (*list)->head = node;
        return;
    }

    prev = NULL;
    while (crt_node != NULL) {
        prev = crt_node;
        crt_node = crt_node->next;
    }

    prev->next = node;
}

void printNode(LinkedListNode *node) {
    printf("key: %s -> value: %s\n", node->key, node->value);
    printf("\n");
}

void printList(LinkedList *list) {
	LinkedListNode *crtNode;

    if ((list == NULL) || (list->head == NULL)) {
        printf("lista vida\n");
        return;
    }

    crtNode = list->head;  

    while (crtNode != NULL) {
        printNode(crtNode);
        crtNode = crtNode->next;
    }
}

LinkedListNode *createNode(char *key, char *value) {
    LinkedListNode *newNode = (LinkedListNode *) calloc(1, sizeof(LinkedListNode));
    if (newNode == NULL) {
        printf("Calloc failed in createNode\n");
        exit(ENOMEM);
    }

    newNode->key = strdup(key);
    if (newNode->key == NULL) {
        printf("Could not duplicate key\n");
        exit(ENOMEM);
    }

    newNode->value = strdup(value);
    if (newNode->value == NULL) {
        printf("Could not duplicate value\n");
        exit(ENOMEM);        
    }

    return newNode;
}

LinkedList *createList() {
    LinkedList *newList = (LinkedList *) calloc(1, sizeof(LinkedList));

    if (newList == NULL) {
        printf("Calloc failed in createList\n");
        exit(ENOMEM);
    }
    newList->head = NULL;

    return newList;
}

void freeNode(LinkedListNode *node) {
    free(node->key);
    free(node->value);
    free(node);
}

void freeList(LinkedList **list) {
    LinkedListNode *crt_node = (*list)->head;

    while (crt_node != NULL) {
        LinkedListNode *aux = crt_node;
        crt_node = crt_node->next;
        freeNode(aux);
    }

    free(*list);
    *list = NULL;
}

// useful in situations like #undef directive
void deleteNode(LinkedList *list, char *key) {
	LinkedListNode *crt_node;
	LinkedListNode *prev;

    if (list == NULL) return;

    crt_node = list->head;

    prev = NULL;
    while (crt_node != NULL) {
        if (strcmp(crt_node->key, key) == 0) {
            break;
        }
        prev = crt_node;
        crt_node = crt_node->next;
    }

    // ELEMENT TO BE DELETED IS THE FIRST IN LIST
    // TODO - FREE THE NODE DELETED
    if (prev == NULL) {
        freeNode(list->head);
        list->head = NULL;
        return;
    } else {
        prev->next = crt_node->next;
    }

    freeNode(crt_node);
}

/*
int main(int argc, char *argv[]) {
    LinkedList* list = createList();
    LinkedListNode *node = createNode(argv[1], argv[2]);
    printNode(node);
    printf("list before insert:\n");
    printList(list);

    insertNode(&list, node);
    printf("list after insert:\n");
    printList(list);

    LinkedListNode *node2 = createNode(argv[3], argv[4]);
    insertNode(&list, node2);
    printf("list after insert of 2 nodes:\n");
    printList(list);

    deleteNode(list, argv[3]);
    printf("list after delete of second node:\n");
    printList(list);

    freeList(&list);
    printList(list);
}
*/