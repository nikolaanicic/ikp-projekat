#include "server_common.h"

Node* queue_heads[HASH_ARRAY_LEN];
Node* main_queue;
HASH_NODE client_hash_array[HASH_ARRAY_LEN];
SOCKET listen_socket;
HANDLE FinishSignal;


HANDLE MainFullSemaphore;
HANDLE MainEmptySemaphore;


/*
	ova funkcija postavlja soket u neblokirajuci mod rada

*/
bool set_non_blocking_mode(SOCKET socket)
{
	unsigned long mode = 1;
	return ioctlsocket(socket, FIONBIO, &mode) == 0;
}
