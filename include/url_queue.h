#ifndef URL_QUEUE_H
#define URL_QUEUE_H

#include <stdbool.h>

typedef struct url_item {
    char *url;
    int depth;
    struct url_item *next;
} url_item_t;

void url_queue_init();
void url_queue_destroy();
void url_queue_push(const char *url, int depth);
bool url_queue_pop(char **url, int *depth); // returns true if popped
bool url_queue_empty();

#endif // URL_QUEUE_H
