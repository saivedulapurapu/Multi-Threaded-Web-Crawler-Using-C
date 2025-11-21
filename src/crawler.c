#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "crawler.h"
#include "url_queue.h"
#include "visited.h"
#include "fetcher.h"
#include "parser.h"
#include "storage.h"

static int g_max_depth = 2;
static int g_num_threads = 4;
static char *g_out_dir = NULL;
static pthread_t *g_workers = NULL;
static volatile int g_running = 0;

static void *worker_loop(void *arg){
    (void)arg;
    while(g_running){
        char *url = NULL;
        int depth = 0;
        if(!url_queue_pop(&url, &depth)){
            /* queue empty: small sleep, then continue */
            usleep(100000);
            continue;
        }
        if(!url) continue;

        /* Check visited */
        if(visited_test_and_set(url)){
            free(url);
            continue;
        }

        /* Fetch */
        size_t len = 0;
        long http_code = 0;
        char *body = fetch_url_to_memory(url, &len, &http_code);
        if(body){
            printf("[thread %lu] fetched %s (code=%ld bytes=%zu)\n", (unsigned long)pthread_self(), url, http_code, len);
            storage_save_page(g_out_dir, url, body, len);

            /* parse links if depth < max */
            if(depth < g_max_depth){
                /* callback will push new URLs with depth+1 */
                void push_cb(const char *found_url, int d, void *ud){
                    (void)d; (void)ud;
                    url_queue_push(found_url, depth+1);
                }
                parse_and_extract_links(url, body, len, depth, push_cb, NULL);
            }

            free(body);
        } else {
            fprintf(stderr, "[thread %lu] failed to fetch %s\n", (unsigned long)pthread_self(), url);
        }

        free(url);
    }
    return NULL;
}

void crawler_init(const char *seed, int max_depth, int num_threads, const char *out_dir){
    g_max_depth = max_depth;
    g_num_threads = num_threads;
    g_out_dir = strdup(out_dir);

    url_queue_init();
    visited_init();

    url_queue_push(seed, 0);

    g_workers = calloc(g_num_threads, sizeof(pthread_t));
    g_running = 1;
    for(int i=0;i<g_num_threads;i++){
        pthread_create(&g_workers[i], NULL, worker_loop, NULL);
    }
}

void crawler_start(){
    /* main thread monitors queue; stops when queue empty and threads idle for some time */
    int idle_cycles = 0;
    while(1){
        if(url_queue_empty()){
            idle_cycles++;
            if(idle_cycles > 20) break; /* ~2 seconds idle => finish */
            usleep(100000);
        } else {
            idle_cycles = 0;
            usleep(100000);
        }
    }
}

void crawler_stop(){
    g_running = 0;
    for(int i=0;i<g_num_threads;i++){
        pthread_join(g_workers[i], NULL);
    }
    free(g_workers);
    url_queue_destroy();
    visited_destroy();
    free(g_out_dir);
}
