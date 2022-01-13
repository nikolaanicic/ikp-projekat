#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")

#include "common.h"
#include <stdlib.h>

HANDLE FinishSignal;
CRITICAL_SECTION console_section;
HANDLE socket_mutex;
DWORD mode;
SOCKET client;


void handle_client_connect_error()
{

	print_last_winsock_error();
	close_winsock();
	exit(-5);
}


/*

	Ova funkcija povezuje klijenta na red na serveru, odnosno uzima od korisnika server na koji treba povezati klijenta
	pravi adresu servera i povezuje klijenta na adresu, ako je tcp vez prihvacena salje blokirajucu config poruku ka serveru sa komandom 
	CONNECT i zeljenim redom i ceka odgovor servera

	Agument:
		queueName -> red na koji treba povezati klijenta

	Povratna vrednost:
		Ako je ostvarena uspesna veza sa redom na serveru onda vraca SOCKET inace vraca SOCKET_ERROR
*/

SOCKET Connect(const char* queueName)
{
	SOCKET client = init_socket();
	SOCKADDR_IN server_address;

	unsigned int server_port = get_server_port();

	populateAddressV4(&server_address, SERVER_IP, server_port);
	connect_to_address(client, server_address);



	MESSAGE* message = make_message_config(map_queue_name_to_type(queueName),_CONNECT_,_CLIENT_,_SERVER_);

	MESSAGE_STATE state = send_message_tcp(client, *message);


	if (state == FAULT)
	{
		handle_client_connect_error();
		free_message(&message);
		close_socket(client);
		return SOCKET_ERROR;
	}

	printf("\nSENT CONFIG CONNECT MESSAGE AWAITING RESPONSE");

	state = receive_message_tcp(client, message);


	if (state == FAULT)
	{
		printf("\nFailed to connect to server queue:%s", queueName);
		free_message(&message);
		close_socket(client);
		return SOCKET_ERROR;
	}

	return client;
}

/*
	Ova funkcija zaustavlja klijenta odnosno salje shutdown kroz klijentski socket i zatvara ga

*/
void stop_client()
{
	shutdown(client, SD_BOTH);
	if (client != INVALID_SOCKET)
		close_socket(client);
}

void handle_init_error_handler()
{
	close_socket(client);
	close_winsock();
	exit(-10);
}
