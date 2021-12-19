#include "Communication.h"
#include <stdio.h>
#include <conio.h>

SOCKET client;
HANDLE FinishSignal;

char get_type_input()
{
	char c = 0;

	do 
	{
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

	init_winsock(MAKEWORD(2, 2));
	
	client = Connect(queueName);

	if(client == SOCKET_ERROR)
	{
		return 1;
	}


	FinishSignal = CreateSemaphore(NULL, 0, 2, NULL);

	if (!FinishSignal)
	{
		printf("\nFailed to initialize Finish signal semaphore");
		handle_init_error_handler();
	}



	sending_thread = CreateThread(NULL, 0, &RunSendingThread, (LPVOID)&type, 0, &sending_thread_id);
	receiving_thread = CreateThread(NULL, 0, &RunAcceptingThread, (LPVOID)NULL, 0, &sending_thread_id);


	if (!sending_thread || !receiving_thread)
	{
		printf("\nFailed to start threads");
		handle_init_error_handler();
	}

	_getch();

	ReleaseSemaphore(FinishSignal, 2, NULL);
		
	WaitForSingleObject(sending_thread,INFINITE);
	WaitForSingleObject(receiving_thread, INFINITE);



	safe_close_handle(FinishSignal);
	safe_close_handle(sending_thread);
	safe_close_handle(receiving_thread);

	close_socket(client);

	close_winsock();

	return 0;
}

