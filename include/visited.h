#ifndef VISITED_H
#define VISITED_H

#include <stdbool.h>

void visited_init();
void visited_destroy();
bool visited_test_and_set(const char *url); // returns true if it was already present

#endif // VISITED_H
