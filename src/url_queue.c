#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "url_queue.h"

static url_item_t *head = NULL;
static url_item_t *tail = NULL;
static pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;

void url_queue_init(){
    head = tail = NULL;
}

void url_queue_destroy(){
    pthread_mutex_lock(&queue_lock);
    url_item_t *p = head;
    while(p){
        url_item_t *n = p->next;
        free(p->url);
        free(p);
        p = n;
    }
    head = tail = NULL;
    pthread_mutex_unlock(&queue_lock);
}

void url_queue_push(const char *url, int depth){
    url_item_t *it = calloc(1, sizeof(url_item_t));
    it->url = strdup(url);
    it->depth = depth;
    it->next = NULL;

    pthread_mutex_lock(&queue_lock);
    if(!tail){
        head = tail = it;
    } else {
        tail->next = it;
        tail = it;
    }
    pthread_mutex_unlock(&queue_lock);
}

bool url_queue_pop(char **url, int *depth){
    bool ok = false;
    pthread_mutex_lock(&queue_lock);
    if(head){
        url_item_t *it = head;
        head = it->next;
        if(!head) tail = NULL;
        *url = it->url;
        *depth = it->depth;
        free(it);
        ok = true;
    } else {
        *url = NULL;
        *depth = 0;
    }
    pthread_mutex_unlock(&queue_lock);
    return ok;
}

bool url_queue_empty(){
    pthread_mutex_lock(&queue_lock);
    bool empty = (head == NULL);
    pthread_mutex_unlock(&queue_lock);
    return empty;
}
