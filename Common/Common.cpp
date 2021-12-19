#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "Ws2_32.lib")


#include "framework.h"
#include <stdio.h>


#pragma region TCP


void print_address(SOCKADDR_IN address)
{
	printf("\nADDRESS:<%s,%d>", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
}


void print_last_winsock_error()
{
	printf("\nError in winsock initialization %d", WSAGetLastError());

}

void init_winsock(WORD version)
{
	WSADATA wsa;

	if (WSAStartup(version, &wsa) != 0)
	{
		print_last_winsock_error();
		exit(-1);
	}
}

void close_winsock()
{
	if (WSACleanup() != 0)
	{
		printf("\nError in winsock cleanup");
		exit(-2);
	}
}

void handle_winsock_error(void (*error_handler)())
{
	(*error_handler)();
}

SOCKET init_socket()
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == SOCKET_ERROR)
	{
		handle_winsock_error(&handle_socket_init_error);
		exit(-3);
	}

	return sock;
}

void handle_socket_close_error()
{
	print_last_winsock_error();
	close_winsock();
}

void handle_socket_init_error()
{
	print_last_winsock_error();
	close_winsock();
}

bool close_socket(SOCKET socket)
{
	bool retval = true;

	if (closesocket(socket) != 0)
	{
		handle_winsock_error(&handle_socket_close_error);
		retval = false;
	}

	return retval;
}


void connect_to_address(SOCKET socket, SOCKADDR_IN address, void (*connect_error_handler)())
{
	int addressSize = sizeof(address);
	int iRes = connect(socket, (SOCKADDR*)&address, addressSize);

	if (iRes != 0)
	{
		handle_winsock_error(connect_error_handler);
	}
}

void populateAddressV4(SOCKADDR_IN* address_struct, const char* ip, short port)
{
	if (address_struct == NULL)
		return;
	else if (ip == NULL)
		return;

	address_struct->sin_addr.S_un.S_addr = inet_addr(ip);
	address_struct->sin_port = htons(port);
	address_struct->sin_family = AF_INET;
}



#pragma endregion


#pragma region HIGHER_LEVEL_CONNECTION

MESSAGE* allocate_message()
{
	MESSAGE* message = (MESSAGE*)malloc(sizeof(MESSAGE));
	if (message == NULL)
	{
		printf("\nFailed to allocate message buffer");

	}

	return message;
}

void free_message(MESSAGE* message)
{
	if (message != NULL)
	{
		free(message);
		message = NULL;
	}
}



MESSAGE_STATE send_message_tcp(SOCKET socket, MESSAGE message)
{
	MESSAGE_STATE retval = FAULT;

	char* send_buffer = allocate_buffer(sizeof(MESSAGE));
	if (!serialize_message(send_buffer, message))
	{
		free(send_buffer);
		printf("\nFailed to serialize message");
		return FAULT;
	}

	int iRes = send(socket, send_buffer, sizeof(MESSAGE), 0);


	if (iRes > 0)
	{
		printf("\nMESSAGE SENT SUCCESSFULLY");
		retval = SUCCESS;
	}
	else if (iRes == SOCKET_ERROR)
	{
		printf("\nMESSAGE NOT SENT");
		print_last_winsock_error();
		retval = FAULT;
	}

	free(send_buffer);

	return retval;
}


MESSAGE_STATE receive_message_tcp(SOCKET s, MESSAGE* message)
{
	MESSAGE_STATE retval = FAULT;

	if (message == NULL)
		return retval;


	char* recv_buffer = allocate_buffer(sizeof(MESSAGE));

	int iRes = recv(s, recv_buffer, sizeof(MESSAGE), 0);

	if (iRes > 0)
	{
		printf("\nMESSAGE SUCCESSFULLY RECEIVED");
		if (deserialize_message(recv_buffer, message))
		{
			printf("\nMESSAGE SUCCESSFULLY DESERIALIZED");
			retval = SUCCESS;
		}
	}
	else if (iRes == 0 || iRes == SOCKET_ERROR)
	{
		if (iRes == 0)
			printf("\nConnection closed");

		close_socket(s);
		retval = FAULT;
	}

	return retval;
}


#pragma endregion


#pragma region MESSAGE_PRINT_HELPERS

void print_buffer(char* buffer, int buffer_len)
{
	if (buffer == NULL || buffer_len <= 0)
		return;

	for (int i = 0; i < buffer_len; i++)
	{
		printf("%c", buffer[i]);
	}
}

void print_data(DATA data, TYPE type)
{
	printf("\nDATA:");
	switch (type)
	{
		case _CHAR_: printf("%c", data.c); break;

		case _INT_:	printf("%d", data.i); break;
		case _SHORT_: printf("%d", data.s); break;
		case _FLOAT_: printf("%f", data.f); break;
		case _DOUBLE_: printf("%.2lf", data.d);  break;
	}
}

void print_command(COMMAND command)
{
	printf("\nCOMMAND:");
	switch (command)
	{
		case _CONNECT_: printf("CONNECT"); break;
		case _DOES_EXIST_: printf("DOES EXIST"); break;
		case _EXISTS_: printf("EXISTS"); break;
		case _MESSAGE_: printf("MESSAGE"); break;
		case _NOT_EXISTS_: printf("NOT EXISTS"); break;
	}
}

void print_location(LOCATION location)
{
	switch (location)
	{
		case _CLIENT_: printf("CLIENT"); break;
		case _SERVER_: printf("SERVER"); break;
	}
}

void print_type(TYPE type)
{
	printf("\nTYPE:");
	switch (type)
	{
		case _INT_: printf("INT"); break;
		case _CHAR_: printf("CHAR"); break;
		case _SHORT_: printf("SHORT"); break;
		case _FLOAT_: printf("FLOAT"); break;
		case _DOUBLE_: printf("DOUBLE"); break;
	}
}


void print_data_message(DATA_MESSAGE message)
{
	print_data(message.data, message.type);
}

void print_config_message(CONFIG_MESSAGE message)
{
	print_command(message.command);
}

void print_inner_message(INNER_MESSAGE message, char has_data)
{
	switch (has_data)
	{
	case 'c': print_config_message(message.config); break;
	case 'd': print_data_message(message.data); break;
	}
}

void print_message(MESSAGE message)
{
	printf("\n*********");
	printf("\nORIGIN:"); print_location(message.origin);
	printf("\nDESTINATION:"); print_location(message.destination);
	printf("\nQUEUE NAME:%s", message.queueName);

	print_inner_message(message.data, message.has_data);
	printf("\n*********");

}



#pragma endregion



#pragma region BUFFER_HELPERS
void handle_no_memory_buffer()
{
	printf("\nNo memory to allocate new message buffer");
}



char* allocate_buffer(int size)
{
	char* buffer = (char*)malloc(size);

	if (buffer == NULL)
	{
		handle_no_memory_buffer();
	}

	return buffer;
}

void free_buffer(char* buffer)
{
	if (buffer != NULL)
	{
		free(buffer);
		buffer = NULL;
	}
}

#pragma endregion



#pragma region MESSAGE_POPULATION_HELPERS


bool serialize_message(void* buffer, MESSAGE message)
{
	if (buffer == NULL)
		return false;

	memcpy(buffer, (char*)&message, sizeof(MESSAGE));

	return true;
}

bool deserialize_message(void* buffer, MESSAGE* message)
{
	if (buffer == NULL || message == NULL)
		return false;

	memcpy(message, buffer, sizeof(MESSAGE));
	return true;
}

bool populate_data(DATA* d, TYPE type, void* data)
{
	if (d == NULL || data == NULL)
		return false;

	switch (type)
	{
		case _CHAR_:
		{		
			char c = *(char*)data;
			d->c = c;
			break;
		}

		case _INT_:
		{
			int i = *(int*)data;
			d->i = i;
			break;

		}
		case _SHORT_:
		{
			short s = *(short*)data;
			d->s = s;
			break;
		}
		case _FLOAT_:
		{
			float f = *(float*)data;
			d->f = f;
			break;
		}
		case _DOUBLE_:
		{
			double dou = *(double*)data;
			d->d = dou;
			break;
		}
		default:
			return false;
	}

	return true;
}


bool populate_config_message(CONFIG_MESSAGE* message,COMMAND command)
{
	if (message == NULL)
		return false;

	message->command = command;


	return true;
}

bool populate_data_message(DATA_MESSAGE* message, TYPE type,void* data)
{
	if (message == NULL || data == NULL)
		return false;

	message->type = type;
	bool retval = populate_data(&(message->data), type, data);
	
	return retval;
}

bool populate_inner_message(INNER_MESSAGE* message, void* data, char has_data)
{
	if (message == NULL || data == NULL)
		return false;

	switch (has_data)
	{
		case 'c':
		{
			memcpy(&(message->config), data, sizeof(CONFIG_MESSAGE));
			break;
		}
		case 'd':
		{
			memcpy(&(message->data), data, sizeof(DATA_MESSAGE));
			break;
		}
	}

	return true;
}


MESSAGE* make_message_config(TYPE queueName, COMMAND command,LOCATION origin,LOCATION destination)
{

	MESSAGE* message = allocate_message();
	CONFIG_MESSAGE config_message;
	populate_config_message(&config_message, command);
	populate_inner_message(&(message->data), (void*)(&config_message), 'c');

	message->has_data = 'c';
	message->destination = destination;
	message->origin = origin;

	strcpy(message->queueName, map_type_to_queue_name(queueName));


	return message;
}

const char* map_type_to_queue_name(TYPE type)
{
	switch (type)
	{
		
		case _INT_: return "int";
		case _CHAR_: return "char";
		case _SHORT_: return "short";
		case _FLOAT_: return "float";
		case _DOUBLE_: return "double";
	}
}

TYPE map_queue_name_to_type(const char* queue_name)
{
	char c = queue_name[0];
	
	return c == 's' ? _SHORT_ : (TYPE)c;

}

MESSAGE* make_message_data(void* data, TYPE type)
{
	MESSAGE* message = allocate_message();

	DATA_MESSAGE data_message;
	populate_data_message(&data_message, type, data);
	populate_inner_message(&(message->data), (void*)&data_message, 'd');

	message->destination = _CLIENT_;
	message->has_data = 'd';
	message->origin = _CLIENT_;
	

	strcpy(message->queueName, map_type_to_queue_name(type));
	return message;
}

#pragma endregion




