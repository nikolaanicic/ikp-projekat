#ifndef _SYNCHRONIZATION_H_
#define _SYNCHRONIZATION_H_

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

HANDLE init_semaphore(int initial_count, int max_count);
void safe_close_handle(HANDLE handle);




#endif
