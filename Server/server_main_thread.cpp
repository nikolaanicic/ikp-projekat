#include "server_main_thread.h"

bool set_non_blocking_mode(SOCKET socket, unsigned long* mode)
{
	return ioctlsocket(socket, FIONBIO, mode) == 0;
}


bool accept_client_connection(SOCKET accepted_connections[], int free_slot,SOCKET listen_socket)
{
	SOCKADDR_IN client_address;
	int size = sizeof(client_address);
	unsigned long mode = 1;

	accepted_connections[free_slot] = accept(listen_socket, (SOCKADDR*)&client_address, &size);

	if (accepted_connections[free_slot] == INVALID_SOCKET)
	{
		printf("\nFailed to accept client connection");
		return false;
	}

	if (!set_non_blocking_mode(accepted_connections[free_slot], &mode))
	{
		printf("\nFailed to set non blocking mode to client socket");
		return false;
	}

	return true;

}


int get_free_client_slot(SOCKET accepted_connections[])
{
	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		if (accepted_connections[i] == INVALID_SOCKET)
			return i;
	}

	return -1;
}


void set_client_sockets(fd_set* fds, SOCKET accepted_connections[])
{
	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		if (accepted_connections[i] != INVALID_SOCKET)
		{
			FD_SET(accepted_connections[i], fds);
		}
	}
}


bool is_connect_message(MESSAGE* message)
{
	if (message == NULL)
		return false;
	return message->has_data == _CONFIG_ && message->data.config.command == _CONNECT_;
}


bool try_connect_client_to_queue(MESSAGE* message,SOCKET socket)
{
	bool retval = false;
	TYPE type = map_queue_name_to_type(message->queueName);

	while (WaitForSingleObject(client_hash_array_mutex, INFINITE) != WAIT_OBJECT_0);

	HASH_NODE node = get_hash_node(type, client_hash_array);

	if (node.socket == INVALID_SOCKET)
	{
		node.socket = socket;
		set_hash_node(type, node, client_hash_array);
		printf("\nClient connected to queue: %s", message->queueName);
		retval = true;
	}
	else
	{
		printf("\nClient connection rejected to queue:%s", message->queueName);
	}

	ReleaseMutex(client_hash_array_mutex);
	return retval;

}


void disconnect_client_from_queue(MESSAGE* message)
{
	TYPE type = map_queue_name_to_type(message->queueName);

	while (WaitForSingleObject(client_hash_array_mutex, INFINITE) != WAIT_OBJECT_0);

	HASH_NODE node = get_hash_node(type, client_hash_array);
	node.socket = INVALID_SOCKET;
	set_hash_node(type, node, client_hash_array);

	ReleaseMutex(client_hash_array_mutex);
}

void push_message_to_server_queue(MESSAGE* message)
{
	while (WaitForSingleObject(server_queue_mutex, INFINITE) != WAIT_OBJECT_0);
	push(&server_queue_head, get_new_node(message));
	ReleaseMutex(server_queue_mutex);
}

void schedule_message(MESSAGE* message,SOCKET client_socket)
{
	if (is_connect_message(message))
	{
		try_connect_client_to_queue(message, client_socket);
	}

	push_message_to_server_queue(message);
}



void check_client_sockets_events(SOCKET accepted_connections[],FD_SET* fds,MESSAGE* buffer)
{
	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		if (accepted_connections[i] != INVALID_SOCKET && FD_ISSET(accepted_connections[i], fds))
		{
			MESSAGE_STATE state = receive_message_tcp(accepted_connections[i], buffer);
			
			if (state == SUCCESS)
			{
				schedule_message(buffer, accepted_connections[i]);
			}
			else if(state == FAULT)
			{
				accepted_connections[i] = INVALID_SOCKET;
			}
			else if (state == DISCONNECT)
			{
				accepted_connections[i] = INVALID_SOCKET;
				disconnect_client_from_queue(buffer);
			}
		}
	}
}


DWORD WINAPI load_balancer(LPVOID lpParam)
{

	SOCKET accepted_connections[MAX_CONNECTIONS];
	memset(accepted_connections, INVALID_SOCKET, sizeof(accepted_connections));

	int next_free = 0;
	FD_SET readfds;

	struct timeval time;
	time.tv_usec = 1;
	MESSAGE* message = allocate_message();

	while (WaitForSingleObject(FinishSignal, 1) != WAIT_OBJECT_0)
	{
		FD_ZERO(&readfds);
		next_free = get_free_client_slot(accepted_connections);

		if (next_free != -1)
		{
			FD_SET(listen_socket, &readfds);
		}

		set_client_sockets(&readfds, accepted_connections);
		
		int select_result = select(0, &readfds, NULL, NULL, &time);

		if (select_result == SOCKET_ERROR)
		{
			print_last_winsock_error();
		}
		else if (select_result == 0)
		{
			// desio se tajmaut
		}
		else if (select_result > 0)
		{
			if (FD_ISSET(listen_socket, &readfds))
			{
				accept_client_connection(accepted_connections, next_free,listen_socket);
			}

			check_client_sockets_events(accepted_connections, &readfds, message);
		}
	}

	free_message(message);
	return 0;
}

MAIN_THREAD_VARS* init_main_thread_vars(SOCKET listen_socket, HANDLE FinishSignal, HANDLE MainServerQueueSemaphore, HANDLE ConnectedClientsArraySempahore, Node* main_server_queue, HASH_NODE* client_hash_array)
{
	MAIN_THREAD_VARS* vars = (MAIN_THREAD_VARS*)malloc(sizeof(MAIN_THREAD_VARS));

	if (vars == NULL)
	{
		printf("\nFailed to allocate main server thread data model");
		return NULL;
	}

	vars->listen_socket = listen_socket;
	vars->FinishSignal = FinishSignal;
	vars->MainServerQueueSemaphore = MainServerQueueSemaphore;
	vars->client_array_mutex = ConnectedClientsArraySempahore;
	vars->main_server_queue_head = main_server_queue;
	vars->client_hash_array = client_hash_array;
	
	return vars;
}

void free_main_thread_vars(MAIN_THREAD_VARS* main_thread_vars)
{
	if (main_thread_vars != NULL)
	{
		free(main_thread_vars);
		main_thread_vars = NULL;
	}
}