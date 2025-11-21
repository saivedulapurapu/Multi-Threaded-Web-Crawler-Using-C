#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include "fetcher.h"

typedef struct {
    char *buf;
    size_t len;
    size_t cap;
} mem_t;

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userdata){
    size_t realsz = size * nmemb;
    mem_t *m = (mem_t*)userdata;
    if(m->len + realsz + 1 > m->cap){
        size_t newcap = (m->cap + realsz + 1) * 2;
        char *nb = realloc(m->buf, newcap);
        if(!nb) return 0;
        m->buf = nb;
        m->cap = newcap;
    }
    memcpy(m->buf + m->len, ptr, realsz);
    m->len += realsz;
    m->buf[m->len] = '\0';
    return realsz;
}

char *fetch_url_to_memory(const char *url, size_t *out_len, long *http_code){
    CURL *curl = curl_easy_init();
    if(!curl) return NULL;

    mem_t m = {NULL, 0, 0};
    m.cap = 4096;
    m.buf = malloc(m.cap);
    m.len = 0;
    m.buf[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "SimpleCrawler/1.0");

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK){
        fprintf(stderr, "curl error: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        free(m.buf);
        return NULL;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, http_code);
    curl_easy_cleanup(curl);

    *out_len = m.len;
    return m.buf; /* caller must free */
}
