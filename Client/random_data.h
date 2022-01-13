#ifndef _RANDOM_H
#define _RANDOM_H

#include "data_model.h"
#define RAND_BUFFER_SIZE (8)

void* get_random_data(TYPE type);
void free_void_buffer(void** buffer);
#endif
