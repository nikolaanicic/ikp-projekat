#include "client_hash_array.h"



/*
	Ova funkcija mapira tip reda na cvor u nizu po modulu HASH_ARRAY_LEN ( 7 )
	
	Svaki red ima svoj cvor i u njemu se nalaze klijentski socket koji je povezan na taj red i semafori 
	koji upravljaju koriscenjem samog reda

*/

int map_type_to_index(TYPE type)
{
	return type % HASH_ARRAY_LEN;
}

bool init_hash_node(HASH_NODE* node)
{
	if (node == NULL)
		return false;

	node->client_mutex = init_not_owned_mutex();
	node->FullSemaphore = init_semaphore(0, 1);
	node->EmptySemaphore = init_semaphore(1, 1);
	node->socket = INVALID_SOCKET;

	return node->client_mutex != NULL;
}
