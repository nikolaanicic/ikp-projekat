#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")

#include "Communication.h"
#include <stdlib.h>

void safe_close_handle(HANDLE a)
{
	if (a)
	{
		CloseHandle(a);
	}
}


extern HANDLE FinishSignal;
extern CRITICAL_SECTION console_section;

#pragma region INTERFACE

extern SOCKET client;

void handle_client_connect_error()
{

	print_last_winsock_error();
	close_winsock();
	exit(-5);
}

SOCKET Connect(const char* queueName)
{
	SOCKET client = init_socket();
	SOCKADDR_IN server_address;

	populateAddressV4(&server_address, SERVER_IP, SERVER_PORT);
	connect_to_address(client, server_address, &handle_client_connect_error);



	MESSAGE* message = make_message_config(map_queue_name_to_type(queueName),_CONNECT_,_CLIENT_,_SERVER_);

	MESSAGE_STATE state = send_message_tcp(client, *message);


	if (state == FAULT)
	{
		handle_client_connect_error();
		free_message(message);
		close_socket(client);
		return SOCKET_ERROR;
	}

	char buffer[BUFFER_SIZE];

	state = receive_message_tcp(client, message);


	if (state == FAULT)
	{
		printf("\nFailed to connect to server queue:%s", queueName);
		free_message(message);
		close_socket(client);
		return SOCKET_ERROR;
	}

	return client;
}

bool Exist(const char* queueName)
{
	MESSAGE* message = make_message_config(map_queue_name_to_type(queueName),_DOES_EXIST_,_SERVER_,_SERVER_);
	MESSAGE_STATE state = send_message_tcp(client, *message);

	bool retval = false;

	if (state == FAULT)
	{
		printf("\nFailed to send EXIST message");
		free_message(message);

		return false;
	}

	printf("\nSUCCESSFULLY SENT EXIST MESSAGE");
	
	state = receive_message_tcp(client, message);

	if (state == FAULT)
	{
		printf("\nFailed to receive message");
		free_message(message);
		return false;
	}

	print_message(*message);
	free_message(message);
	
	return true;
}


bool Send_Message(void* message, TYPE type)
{
	MESSAGE* msg = make_message_data(message, type);
	MESSAGE_STATE state = send_message_tcp(client, *msg);

	if (state == FAULT)
	{
		printf("\nFAILED TO SEND MESSAGE TO SERVICE");
		free_message(msg);
		return false;
	}

	free_message(msg);

	return true;
}



DWORD WINAPI RunAcceptingThread(LPVOID lpParam)
{
	MESSAGE* message = allocate_message();
	printf("\nStarted client accepting thread");

	while (WaitForSingleObject(FinishSignal,1000) != WAIT_OBJECT_0)
	{
		MESSAGE_STATE state = receive_message_tcp(client, message);
		EnterCriticalSection(&console_section);
		print_message(*message);
		LeaveCriticalSection(&console_section);
	
	}

	free_message(message);

	return 0;
}


DWORD WINAPI RunSendingThread(LPVOID lpParam)
{
	TYPE type = *(TYPE*)lpParam;

	MESSAGE* message = allocate_message();
	
	printf("\nStarted client sending thread");
	while (WaitForSingleObject(FinishSignal, 5000) != WAIT_OBJECT_0)
	{
	
		void* random_data = get_random_data(type);

		message = make_message_data(random_data, type);

		MESSAGE_STATE state = send_message_tcp(client, *message);

		if (state == FAULT)
		{
			printf("\nFailed to send DATA MESSAGE");

		}
		else
		{
			EnterCriticalSection(&console_section);
			printf("\nSUCCESSFULLY SENT DATA MESSAGE");
			print_message(*message);
			EnterCriticalSection(&console_section);
		}

		free_void_buffer(random_data);

	}

	free_message(message);

	return 0;
}



#pragma endregion




void handle_init_error_handler()
{
	close_socket(client);
	close_winsock();
	exit(-10);
}
