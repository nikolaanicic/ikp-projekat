#include "common.h"
#include <stdio.h>
#include <conio.h>
#include "client_config.h"


char get_type_input()
{
	char c = 0;

	do 
	{
		printf("\ni-INT c-CHAR f-FLOAT d-DOUBLE h-SHORT");
		printf("\nUnesite zeljeni tip klijenta:");
		scanf_s("%c", &c);
	} while (c != 'i' && c != 'c' && c != 'f' && c != 'd' && c != 'h');

	return c;
}


int main()
{
	DWORD sending_thread_id;
	DWORD receiving_thread_id;

	HANDLE sending_thread;
	HANDLE receiving_thread;

	char type = get_type_input();
	const char* queueName = map_type_to_queue_name((TYPE)type);
	TYPE data_type = map_queue_name_to_type(queueName);


	init_winsock(MAKEWORD(2, 2));
	
	client = Connect(queueName);

	if(client == SOCKET_ERROR)
	{
		return 1;
	}

	InitializeCriticalSection(&console_section);
	FinishSignal = init_semaphore(0, 2);
	socket_mutex = init_not_owned_mutex();

	mode = get_work_mode();

	sending_thread = CreateThread(NULL, 0, &RunSendingThread, (LPVOID)&data_type, 0, &sending_thread_id);
	receiving_thread = CreateThread(NULL, 0, &RunAcceptingThread, (LPVOID)NULL, 0, &receiving_thread_id);


	if (!sending_thread || !receiving_thread)
	{
		printf("\nFailed to start threads");
		handle_init_error_handler();
	}

	if (mode == _MENU_) {
		while (WaitForSingleObject(FinishSignal, INFINITE) != WAIT_OBJECT_0);
	}
	else if (mode == _STRESS_TEST_)
	{
		printf("\nPress [Enter] to stop the client:");
		_getch();
	}

	ReleaseSemaphore(FinishSignal, 2, NULL);

	safe_close_handle(FinishSignal);
	safe_close_handle(sending_thread);
	safe_close_handle(receiving_thread);
	DeleteCriticalSection(&console_section);

	stop_client();
	close_winsock();

	return 0;
}

