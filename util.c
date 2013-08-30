#include "shoelaces.h"

void *
memzero(void *p, size_t length)
{
        return memset(p, 0, length);
}
