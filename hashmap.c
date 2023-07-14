#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "hashmap.h"

int hashCode(Hashmap *hashmap, char *key) {
   // it's far from a perfect hashcode,
   // but this i snot the objective of this homework
   return key[0] % hashmap->noBuckets;
}

int getNoKeys(Hashmap *hashmap) {
    int it;
    int no_keys = 0;

    for (it = 0; it < hashmap->noBuckets; it++) {
        no_keys += getSize(hashmap->entries[it]);
    }

    return no_keys;
}

void insert(Hashmap *hashmap, char *key, char *value) {

    int hashIndex = hashCode(hashmap, key);

    LinkedListNode *newNode = createNode(key, value);

    insertNode(&hashmap->entries[hashIndex], newNode);
}

void removeKey(Hashmap *hashmap, char *key) {
	LinkedListNode *crt_node;
    int hashIndex = hashCode(hashmap, key);

    if (hashmap->entries[hashIndex] == NULL) return ;

    crt_node = hashmap->entries[hashIndex]->head;
    while(crt_node != NULL) {
        if (strcmp(crt_node->key, key) == 0) {
            deleteNode(hashmap->entries[hashIndex], key);
            break;
        }
        crt_node = crt_node->next;
    }    

    return;
}

char *getValue(Hashmap *hashmap, char *key) {
   int hashIndex = hashCode(hashmap, key);  
	

   LinkedList *list = hashmap->entries[hashIndex];

    LinkedListNode *crt_node = list->head;

   while(crt_node != NULL) {
       if (strcmp(crt_node->key, key) == 0) {
           return crt_node->value;
           break;
       }
       crt_node = crt_node->next;
   }        
	
   return NULL;        
}



Hashmap *createHashmap(int noBuckets) {
	int it;

    Hashmap *hashmap = (Hashmap *) calloc(1, sizeof(Hashmap));
    if (hashmap == NULL) {
        printf("Error at allocating hashmap\n");
        exit(ENOMEM);
    }

    hashmap->entries = (LinkedList **) calloc(noBuckets, sizeof(LinkedList *));
    if (hashmap->entries == NULL) {
        printf("Error at allocating hashmaps' entries\n");
        exit(ENOMEM);
    }

    for (it = 0; it < noBuckets; it++) {
        hashmap->entries[it] = (LinkedList *) calloc(1, sizeof(LinkedList));
        if (hashmap->entries[it] == NULL) {
            printf("Error at allocating hashmaps' entry, number %d\n", it);
            exit(ENOMEM);
        }
    }

    hashmap->noBuckets = noBuckets;

    return hashmap;
}

void printHashmap(Hashmap *hashmap) {
	int i;
    if (hashmap == NULL) return;

    printf("There are %d buckets in the hashmap\n", hashmap->noBuckets);

    for (i = 0; i < hashmap->noBuckets; i++) {
        printf("pentru intrarea: %d avem lista:\n", i);
        printList(hashmap->entries[i]);
    }
}

void freeHashmap(Hashmap *hashmap) {
	int it;
    if (hashmap == NULL) return;

    for (it = 0; it < hashmap->noBuckets; it++) {
        freeList(&hashmap->entries[it]);
    }

    free(hashmap->entries);
    free(hashmap);
}

/*
int main(int argc, char *argv[]) {
    Hashmap *hashmap = createHashmap(DEFAULT_NO_BUCKETS);
    insert(hashmap, "DOI", "2");
    insert(hashmap, "NEG", "0");
    insert(hashmap, "TRUE", "1");
    insert(hashmap, "NOT", "0");
    printHashmap(hashmap);

    freeHashmap(hashmap);
}
*/