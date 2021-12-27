#include "server_config.h"


SOCKET listen_socket;

void deallocate_server_resources()
{
	safe_close_handle(FinishSignal);
	safe_close_handle(server_queue_mutex);
	for (int i = 0; i < HASH_ARRAY_LEN; i++)
	{
		safe_close_handle(client_hash_array_mutex[i]);
	}


	free_queue(&server_queue_head);
	close_socket(listen_socket);

	//ovde dealocirati i worker threadove kao i server_main_thread
}

void allocate_server_resources()
{
	init_hash_array(client_hash_array);
	init_queue(&server_queue_head);
	FinishSignal = init_semaphore(0, 6);
	server_queue_mutex = init_mutex();

	if (!FinishSignal || server_queue_head != NULL || !server_queue_mutex)
	{
		printf("\nFailed to initialize server's resources");
		deallocate_server_resources();
		close_winsock();
		exit(-12);
	}

	for (int i = 0; i < HASH_ARRAY_LEN; i++)
	{
		client_hash_array_mutex[i] = init_mutex();
		if (client_hash_array_mutex[i] == NULL)
			stop_server();
	}

	//ovde alocirati sve resurse za server threadove, sockete, redove
	

}


unsigned short get_server_port()
{
	unsigned short server_port;

	do
	{
		printf("\nUnesite redni broj servera:");

		int num;
		scanf_s("%d", &num);

		if (num == 1)
			server_port = SERVER_PORT_1;
		else if (num == 2)
			server_port = SERVER_PORT_2;
		else
			server_port = 1;

	} while (server_port != SERVER_PORT_1 && server_port != SERVER_PORT_2);

	return server_port;
}


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

	return true;
}


bool configure_server_socket(SOCKADDR_IN server_address)
{
	unsigned long mode = 1;
	listen_socket = init_socket();

	if (listen_socket == SOCKET_ERROR)
	{
		handle_socket_init_error();
		return false;
	}
	else if (!set_non_blocking_mode(listen_socket) || !bind_server_socket(server_address) || listen(listen_socket, SOMAXCONN) != 0)
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

	allocate_server_resources();

	return true;
}


void stop_server()
{
	shutdown(listen_socket, SD_BOTH);
	ReleaseSemaphore(FinishSignal, 1, NULL);
	
	deallocate_server_resources();
}

void  run_server()
{

}