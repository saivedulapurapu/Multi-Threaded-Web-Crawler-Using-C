#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>   // for size_t

// Extracts URLs from the provided html buffer. For each found url, calls cb(url, depth).
typedef void (*url_found_cb)(const char *url, int depth, void *userdata);

void parse_and_extract_links(const char *base_url, const char *html, size_t len, int current_depth, url_found_cb cb, void *userdata);

#endif // PARSER_H
