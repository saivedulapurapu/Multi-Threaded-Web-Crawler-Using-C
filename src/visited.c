/* Simple thread-safe chained-hash visited set */

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "visited.h"

#define TABLE_SIZE 10007

typedef struct node {
    char *url;
    struct node *next;
} node_t;

static node_t *table[TABLE_SIZE];
static pthread_mutex_t table_lock = PTHREAD_MUTEX_INITIALIZER;

static unsigned long djb2(const char *str){
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

void visited_init(){
    for(int i=0;i<TABLE_SIZE;i++) table[i] = NULL;
}

void visited_destroy(){
    for(int i=0;i<TABLE_SIZE;i++){
        node_t *n = table[i];
        while(n){
            node_t *tmp = n->next;
            free(n->url);
            free(n);
            n = tmp;
        }
    }
}

bool visited_test_and_set(const char *url){
    unsigned long h = djb2(url) % TABLE_SIZE;
    pthread_mutex_lock(&table_lock);
    node_t *n = table[h];
    while(n){
        if(strcmp(n->url, url) == 0){
            pthread_mutex_unlock(&table_lock);
            return true;
        }
        n = n->next;
    }
    /* not found: insert */
    node_t *ins = malloc(sizeof(node_t));
    ins->url = strdup(url);
    ins->next = table[h];
    table[h] = ins;
    pthread_mutex_unlock(&table_lock);
    return false;
}
