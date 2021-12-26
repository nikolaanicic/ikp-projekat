#ifndef _SERVER_MAIN_THREAD_H
#define _SERVER_MAIN_THREAD_H

#include "winsock_helpers.h"
#include "queue.h"
#include <Windows.h>
#include "server_config.h"
#include "high_level_connection.h"
#include "client_hash_array.h"
#include "server_common.h"


typedef struct _main_thread_data_
{
	SOCKET listen_socket;
	HANDLE FinishSignal;
	HANDLE MainServerQueueSemaphore;
	HANDLE client_array_mutex;
	Node* main_server_queue_head;
	HASH_NODE* client_hash_array;
}MAIN_THREAD_VARS;


extern SOCKET listen_socket;

DWORD WINAPI load_balancer(LPVOID lpParam);

MAIN_THREAD_VARS* init_main_thread_vars(SOCKET listen_socket, HANDLE FinishSignal, HANDLE MainServerQueueSemaphore, HANDLE ConnectedClientsArraySempahore,Node* main_server_queue,HASH_NODE* client_hash_array);
void free_main_thread_vars(MAIN_THREAD_VARS* main_thread_vars);

#endif
