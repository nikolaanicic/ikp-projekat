#ifndef _REQUEST_WORKER_H
#define _REQUEST_WORKER_H

#include <WinSock2.h>
#include <Windows.h>
#include "queue.h"
#include "high_level_connection.h"
#include "client_hash_array.h"
#include "server_common.h"

typedef struct _worker_params_
{
	SOCKET* client_socket;
	HANDLE client_mutex;
	TYPE queue_type;
	SOCKADDR_IN brother_address;

}WORKER_PARAMS;

DWORD WINAPI request_worker(LPVOID lpParam);

void make_worker_params(WORKER_PARAMS* output,SOCKET* client_socket, HANDLE client_mutex, TYPE queue_type,SOCKADDR_IN brother_address);
void free_worker_params(WORKER_PARAMS** data);


#endif