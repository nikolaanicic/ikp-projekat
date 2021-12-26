#include "client_hash_array.h"



int map_type_to_index(TYPE type)
{
	return type % HASH_ARRAY_LEN;
}

void init_hash_array(HASH_NODE client_array[])
{
	for (int i = 0; i < HASH_ARRAY_LEN; i++)
	{
		client_array[i].socket = INVALID_SOCKET;
	}
}

void set_hash_node(TYPE type, HASH_NODE node,HASH_NODE client_array[])
{
	int index = map_type_to_index(type);
	memcpy(&client_array[index], &node, sizeof(HASH_NODE));
	
}

HASH_NODE get_hash_node(TYPE type,HASH_NODE client_array[])
{
	int index = map_type_to_index(type);
	return client_array[index];
}
