#ifndef _SERVER_COMMON_H_
#define _SERVER_COMMON_H_

#include <WinSock2.h>
#include "client_hash_array.h"
#include "queue.h"
#include "data_model.h"

extern Node* queue_heads[HASH_ARRAY_LEN];
extern Node* main_queue;
extern HASH_NODE client_hash_array[HASH_ARRAY_LEN];
extern HANDLE FinishSignal;
extern SOCKET listen_socket;

extern HANDLE MainFullSemaphore;
extern HANDLE MainEmptySemaphore;

enum tuple 
{ 
	_SERVER_SERVER_, 
	_SERVER_CLIENT_, 
	_CLIENT_SERVER_, 
	_CLIENT_CLIENT_
};

bool set_non_blocking_mode(SOCKET socket);

#endif