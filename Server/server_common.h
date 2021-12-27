#ifndef _SERVER_COMMON_H_
#define _SERVER_COMMON_H_

#include <WinSock2.h>
#include "client_hash_array.h"
#include "queue.h"

extern HANDLE FinishSignal;
extern HASH_NODE client_hash_array[HASH_ARRAY_LEN];
extern Node* server_queue_head;
extern HANDLE server_queue_mutex;
extern HANDLE client_hash_array_mutex[HASH_ARRAY_LEN];


bool set_non_blocking_mode(SOCKET socket);

#endif