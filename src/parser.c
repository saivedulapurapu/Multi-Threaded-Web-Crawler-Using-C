/* Very small link extractor. Not a full HTML parser.
   Looks for href="..." and absolute/relative links and makes them absolute (basic). */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

/* basic join: if link starts with http(s) => return copy; if starts with '/', prepend scheme+host; else naive join base path. */
static char *make_absolute(const char *base, const char *link){
    if(!link || strlen(link)==0) return NULL;
    if(strncmp(link, "http://", 7)==0 || strncmp(link, "https://", 8)==0){
        return strdup(link);
    }
    /* find scheme://host in base */
    const char *p = strstr(base, "://");
    if(!p) return strdup(link);
    p += 3;
    const char *slash = strchr(p, '/');
    size_t hostlen = slash ? (size_t)(slash - base) : strlen(base);
    if(link[0] == '/'){
        /* absolute path on same host */
        char *r = malloc(hostlen + strlen(link) + 1);
        memcpy(r, base, hostlen);
        r[hostlen] = '\0';
        strcat(r, link);
        return r;
    } else {
        /* relative path — take base up to last '/' */
        const char *last_slash = strrchr(base, '/');
        size_t prefix = last_slash ? (size_t)(last_slash - base) + 1 : strlen(base);
        char *r = malloc(prefix + strlen(link) + 1);
        memcpy(r, base, prefix);
        r[prefix] = '\0';
        strcat(r, link);
        return r;
    }
}

/* naive href finder */
void parse_and_extract_links(const char *base_url, const char *html, size_t len, int current_depth, url_found_cb cb, void *userdata){
    (void)len; (void)current_depth;
    const char *p = html;
    while((p = strstr(p, "href"))){
        /* move to = */
        const char *eq = strchr(p, '=');
        if(!eq) { p += 4; continue; }
        /* find quote */
        const char *q = eq+1;
        while(*q && (*q==' ' || *q=='\t')) q++;
        if(*q == '"' || *q == '\''){
            char quote = *q;
            q++;
            const char *end = strchr(q, quote);
            if(!end){ p = q; continue; }
            size_t linklen = (size_t)(end - q);
            if(linklen == 0){ p = end+1; continue; }
            char *link = malloc(linklen+1);
            memcpy(link, q, linklen);
            link[linklen] = '\0';

            /* basic filter: ignore mailto, javascript, fragments */
            if(strncmp(link, "mailto:", 7) != 0 && strncmp(link, "javascript:", 11) != 0 && link[0] != '#'){
                char *abs = make_absolute(base_url, link);
                if(abs){
                    cb(abs, current_depth + 1, userdata);
                    free(abs);
                }
            }
            free(link);
            p = end+1;
        } else {
            p = eq+1;
        }
    }
}
