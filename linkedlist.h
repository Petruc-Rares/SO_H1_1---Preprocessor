typedef struct LinkedListNode LinkedListNode;
typedef struct LinkedList LinkedList;

struct LinkedListNode {
    char *key;
    char *value;
    LinkedListNode *next;
};

struct LinkedList {
    LinkedListNode *head;
};

int getSize(LinkedList *list);
void insertNode(LinkedList **list, LinkedListNode *node);
void printNode(LinkedListNode *node);
void printList(LinkedList *list);
LinkedListNode *createNode(char *key, char *value);
LinkedList *createList();
void freeNode(LinkedListNode *node);
void freeList(LinkedList **list);
void deleteNode(LinkedList *list, char *key);