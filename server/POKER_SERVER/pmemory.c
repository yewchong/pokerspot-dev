#include "includes.h"
#include "pmemory.h"

void *Pmalloc(size_t size)
{
	return malloc(size);
};