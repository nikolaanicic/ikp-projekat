#include "server_config.h"

/*
	Ova funkcija zatvara prosledjeni niz HANDLE objekata odredjene duzine

*/
void destory_handle_array(HANDLE* handles,int length)
{
	for (int i = 0; i < length; i++)
	{
		safe_close_handle(handles[i]);
	}
}

/*
	Ova funkcija oslobadja sve tipske redove podataka

*/
void destory_queues()
{
	for (int i = 0; i < HASH_ARRAY_LEN; i++)
	{
		free_queue(&queue_heads[i]);
	}
}


/*
	Ova funkcija radi gracefull shutdown servera
	Zatvara svaki handle semafora i mutexa, oslobadja i brise sve redove podataka kao i sve dinamicki alocirane podatke na serveru

*/
void safe_close_server()
{
	ReleaseSemaphore(FinishSignal, 7, NULL);
	safe_close_handle(FinishSignal);
	destory_queues();
	close_socket(listen_socket);
	close_winsock();
	exit(0);
}




/*
	Ova funkcija inicijalizuje sve redove podataka

*/
void init_queues()
{
	for (int i = 0; i < HASH_ARRAY_LEN; i++)
	{
		init_queue(&queue_heads[i]);
	}
}


/*
	Ova funkcija zauzima niz parametara za worker threadove 

	Povratna vrednost:
		pokazivac na zauzet blok memorije odnosno NULL ako je zauzimanje neuspesno

*/
WORKER_PARAMS* allocate_worker_params(int len)
{
	return (WORKER_PARAMS*)malloc(sizeof(WORKER_PARAMS) * len);
}



/*
	Ova funkcija radi zauzimanje svih resursa na serveru

	Zauzima mutexe i semafore za redove, mutexe za klijentske sockete, FinishSignal semaphore


	Nema povratnu vrednost

*/
void allocate_resources()
{
	init_queues();
	init_queue(&main_queue);


	if (((FinishSignal = init_semaphore(0, WORKER_NUM + 2)) == NULL))
	{
		safe_close_server();
	}

	for (int i = 0; i < HASH_ARRAY_LEN; i++)
	{
		if (!init_hash_node(&client_hash_array[i]))
		{
			safe_close_server();
		}
	}

	MainFullSemaphore = init_semaphore(0, 1);
	MainEmptySemaphore = init_semaphore(1, 1);

	if (!MainFullSemaphore || !MainEmptySemaphore)
	{
		safe_close_server();
	}


}


void handle_server_config_error()
{
	print_last_winsock_error();
	close_socket(listen_socket);
}


/*
	Ova funkcija bind-uje serverski socket za prosledjenu adresu

	Povratna vrednost:
		true/false

*/
bool bind_server_socket(SOCKADDR_IN server_address)
{
	if (bind(listen_socket, (SOCKADDR*)&server_address, sizeof(SOCKADDR_IN)) != 0)
	{
		handle_server_config_error();
		return false;
	}

	return true;
}

/*
	Ova funkcija konfigurise serverski socket, odnosno postavlja neblokirajuci mod rada, binduje ga za adresu servera i postavlja ga u listen
	mod rada

	Povratna vrednost:
		true/false

*/
bool configure_server_socket(SOCKADDR_IN my_address)
{
	bool retval = true;
	listen_socket = init_socket();
	if (listen_socket == INVALID_SOCKET)
	{
		handle_socket_init_error();
		retval = false;
	}
	else if (!set_non_blocking_mode(listen_socket) || !bind_server_socket(my_address) || listen(listen_socket, SOMAXCONN) != 0)
	{
		handle_server_config_error();
		retval = false;
	}

	if (retval) {
		printf("\nSERVER IS LISTENING:");
		print_address(my_address);
	}

	return retval;
}

/*
	Ova funkcija sluzi kao pokretac servera, inicijalizuje winsock, alocira resurse za server, pravi adresu servera i adresu bratskog servera

	Povratna vrednost:
		adresa bratskog servera
*/
SOCKADDR_IN server_starter()
{
	init_winsock(MAKEWORD(2, 2));
	allocate_resources();


	unsigned short my_port = get_server_port();
	unsigned short brother_port = my_port == SERVER_PORT_1 ? SERVER_PORT_2 : SERVER_PORT_1;

	SOCKADDR_IN my_address, brother_address;
	populateAddressV4(&my_address, "127.0.0.1", my_port);
	populateAddressV4(&brother_address, "127.0.0.1", brother_port);

	if (!configure_server_socket(my_address))
	{
		printf("\nFailed to correctly set up server's listening socket. Exiting");
		close_socket(listen_socket);
		close_winsock();
		safe_close_server();
		exit(-2);
	}

	return brother_address;
}





