#include "server_common.h"

HANDLE FinishSignal;
HASH_NODE client_hash_array[HASH_ARRAY_LEN];
Node* server_queue_head;
HANDLE server_queue_mutex;
HANDLE client_hash_array_mutex[HASH_ARRAY_LEN];


bool set_non_blocking_mode(SOCKET socket)
{
	unsigned long mode = 1;
	return ioctlsocket(socket, FIONBIO, &mode) == 0;
}
