#ifndef STORAGE_H
#define STORAGE_H

#include <stddef.h>   // for size_t

// Save a buffer to disk; returns 0 on success.
int storage_save_page(const char *out_dir, const char *url, const char *contents, size_t len);

#endif // STORAGE_H
