#include "synchronization.h"

HANDLE init_semaphore(int initial_count, int max_count)
{

	HANDLE semaphore = CreateSemaphore(NULL, initial_count, max_count, NULL);

	if (!semaphore)
	{
		printf("\nFailed to initialize semaphore");
		exit(-19);
	}

	return semaphore;
}

void safe_close_handle(HANDLE handle)
{
	if (handle)
	{
		CloseHandle(handle);
	}
}