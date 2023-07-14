#include "linkedlist.h"

#define DEFAULT_NO_BUCKETS 10

typedef struct Hashmap Hashmap;

struct Hashmap {
    LinkedList** entries;
    int noBuckets;
};

int getNoKeys(Hashmap *hashmap);
int hashCode(Hashmap *hashmap, char *key);
void insert(Hashmap *hashmap, char *key, char *value);
char *getValue(Hashmap *hashmap, char *key);
Hashmap *createHashmap(int noBuckets);
void printHashmap(Hashmap *hashmap);
void freeHashmap(Hashmap *hashmap);
void removeKey(Hashmap *hashmap, char *key);