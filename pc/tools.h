#ifndef TOOLS_H
#define TOOLS_H

#include <stdint.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define LIMIT(x, low, high) MIN(MAX(x, low), high)

#endif
