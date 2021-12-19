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


#pragma region RANDOM_DATA
#define RAND_BUFFER_SIZE (8)


char* generateRandomData()
{
	int l = rand();
	int r = rand();

	char* buffer = allocate_buffer(RAND_BUFFER_SIZE);

	memcpy(buffer, &l, sizeof(int));
	memcpy(&(buffer[1]), &r, sizeof(int));

	return buffer;
}


void free_void_buffer(void* buffer)
{
	if (buffer != NULL)
	{
		free(buffer);
		buffer = NULL;
	}
}

int get_random_int(char* buffer)
{
	return ((int*)buffer)[rand() % (RAND_BUFFER_SIZE / sizeof(int))];
}

char get_random_char(char* buffer)
{
	return buffer[rand() % RAND_BUFFER_SIZE];
}

short get_random_short(char* buffer)
{
	return ((short*)buffer)[rand() % (RAND_BUFFER_SIZE / sizeof(short))];
}

float get_random_float(char* buffer)
{
	return ((float*)buffer)[rand() % (RAND_BUFFER_SIZE / sizeof(float))];
}

double get_random_double(char* buffer)
{
	return *((double*)buffer);
}


void* allocate_void_buffer(int size)
{
	void* buffer = malloc(size);

	if (buffer == NULL)
	{
		printf("\nFailed to allocate buffer");
	}

	return buffer;
}

void* get_random_data(char type)
{
	char* random_buffer = generateRandomData();
	void* data_buffer = NULL;
	switch (type)
	{
	case 'c':
	{
		data_buffer = allocate_void_buffer(sizeof(char));
		char c = get_random_char(random_buffer);
		memcpy(data_buffer, &c, sizeof(char));
		break;
	}
	case 'i':
	{
		data_buffer = allocate_void_buffer(sizeof(int));
		int i = get_random_int(random_buffer);
		memcpy(data_buffer, &i, sizeof(int));
		break;
	}
	case 's':
	{
		data_buffer = allocate_void_buffer(sizeof(short));
		short s = get_random_short(random_buffer);
		memcpy(data_buffer, &s, sizeof(short));
		break;
	}
	case 'f':
	{
		data_buffer = allocate_void_buffer(sizeof(float));
		float f = get_random_float(random_buffer);
		memcpy(data_buffer, &f, sizeof(float));
		break;
	}
	case 'd':
	{
		data_buffer = allocate_void_buffer(sizeof(double));
		double d = get_random_float(random_buffer);
		memcpy(data_buffer, &d, sizeof(double));
		break;
	}
	}

	free_buffer(random_buffer);
	return data_buffer;
}

#pragma endregion


extern HANDLE FinishSignal;


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
		print_message(*message);
	
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
			printf("\nSUCCESSFULLY SENT DATA MESSAGE");
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
