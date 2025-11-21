#include <stdio.h>
#include <stdlib.h>
#include "crawler.h"

int main(int argc, char **argv){
    if(argc != 5){
        fprintf(stderr, "Usage: %s seed_url max_depth num_threads output_dir\n", argv[0]);
        return 1;
    }

    const char *seed = argv[1];
    int max_depth = atoi(argv[2]);
    int num_threads = atoi(argv[3]);
    const char *out_dir = argv[4];

    if(max_depth < 0) max_depth = 2;
    if(num_threads <= 0) num_threads = 4;

    crawler_init(seed, max_depth, num_threads, out_dir);
    crawler_start();
    crawler_stop();

    return 0;
}
