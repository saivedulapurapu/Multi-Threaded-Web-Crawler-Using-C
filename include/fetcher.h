#ifndef FETCHER_H
#define FETCHER_H

#include <stddef.h>   // for size_t

// fetches content into a malloc'd buffer, sets *out_len. Returns NULL on error.
char *fetch_url_to_memory(const char *url, size_t *out_len, long *http_code);

#endif // FETCHER_H
