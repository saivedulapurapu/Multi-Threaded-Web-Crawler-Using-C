#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "storage.h"

/* create directory if not exists */
static int ensure_dir(const char *dir){
    struct stat st;
    if(stat(dir, &st) != 0){
        /* try to create */
        if(mkdir(dir, 0755) != 0){
            perror("mkdir");
            return -1;
        }
    }
    return 0;
}

/* very simple sanitizer for filename */
static void url_to_filename(const char *url, char *out, size_t outlen){
    /* remove scheme */
    const char *p = strstr(url, "://");
    if(p) p += 3; else p = url;
    /* replace non-alnum with _ */
    size_t i=0;
    for(; *p && i+1<outlen; p++){
        char c = *p;
        if((c >= 'a' && c <= 'z') || (c>='A' && c<='Z') || (c>='0' && c<='9')){
            out[i++] = c;
        } else {
            out[i++] = '_';
        }
    }
    out[i] = '\0';
}

int storage_save_page(const char *out_dir, const char *url, const char *contents, size_t len){
    if(ensure_dir(out_dir) != 0) return -1;

    char fname[1024];
    url_to_filename(url, fname, sizeof(fname));
    char path[2048];
    snprintf(path, sizeof(path), "%s/%s.html", out_dir, fname);

    FILE *f = fopen(path, "wb");
    if(!f) {
        perror("fopen");
        return -1;
    }
    fwrite(contents, 1, len, f);
    fclose(f);
    return 0;
}
