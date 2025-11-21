#ifndef CRAWLER_H
#define CRAWLER_H

#include <stdbool.h>

void crawler_init(const char *seed, int max_depth, int num_threads, const char *out_dir);
void crawler_start();
void crawler_stop();
bool is_valid_url(const char *url);

#endif // CRAWLER_H
