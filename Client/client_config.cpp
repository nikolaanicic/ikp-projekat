#include "client_config.h"

WORK_MODE get_work_mode()
{
	int work_mode = -1;

	do
	{
		EnterCriticalSection(&console_section);
		printf("\n1.Menu");
		printf("\n2.Stress test");
		printf("\n");
		LeaveCriticalSection(&console_section);
		scanf_s("%d", &work_mode);

		if (work_mode != (int)_MENU_ && work_mode != _STRESS_TEST_)
		{
			work_mode = -1;
		}

	} while (work_mode == -1);

	return (WORK_MODE)work_mode;
}