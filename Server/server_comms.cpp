#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")


#include "server_comms.h"
#include <stdbool.h>


extern HASH_NODE client_array[HASH_ARRAY_LEN];


#pragma region SERVER_DATA_MODEL


int map_type_to_index(TYPE type)
{
	return type % HASH_ARRAY_LEN;
}

void init_hash_array()
{

	for (int i = 0; i < HASH_ARRAY_LEN; i++)
	{
		client_array[i].socket = -1;
		memset(&(client_array[i].client_address), 0, sizeof(SOCKADDR_IN));
	}
}

#pragma endregion


extern SOCKET listen_socket;
extern HANDLE FinishSignal;


#pragma region SERVER_CONFIG

void handle_server_config_error()
{
	print_last_winsock_error();
	close_socket(listen_socket);
}

bool bind_server_socket(SOCKADDR_IN server_address)
{
	if (bind(listen_socket, (SOCKADDR*)&server_address, sizeof(SOCKADDR_IN)) != 0)
	{
		handle_server_config_error();
		return false;
	}

	printf("\nSERVER BOUND");
	print_address(server_address);
	
	return true;
}


bool configure_server_socket(SOCKADDR_IN server_address)
{

	listen_socket = init_socket();

	if (listen_socket == SOCKET_ERROR)
	{
		handle_socket_init_error();
		return false;
	}

	printf("\nCreated server's listening socket");


	if (!bind_server_socket(server_address))
	{
		return false;
	}

	if (listen(listen_socket, SOMAXCONN) != 0)
	{
		handle_server_config_error();
		return false;
	}

	printf("\nSERVER IS LISTENING:");
	print_address(server_address);

	return true;
}

bool init_server(SOCKADDR_IN address)
{

	if (!configure_server_socket(address))
	{
		printf("\nFailed to configure server's listening socket");
		return false;
	}

	init_hash_array();

	return true;
}

#pragma endregion




DWORD WINAPI load_balancer(LPVOID lpParam)
{
	SOCKET accepted_connections[MAX_CONNECTIONS];

	// napraviti server tako da serveri imaju boot up sekvencu
	// odnosno kada se pokrecu, proverava se postojanje drugog servera
	// uspostave se veze izmedju servera ako su obojica online
	// odnosno povezu se load balanceri i ostave te konekcije aktivnim da 
	// bi thread poolovi mogli da gadjaju load balancer sa suprotnog servisa
	

	return 0;
}