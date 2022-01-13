#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")

#include "Communication.h"
#include <stdlib.h>

extern HANDLE FinishSignal;
extern CRITICAL_SECTION console_section;
extern HANDLE socket_mutex;

#pragma region INTERFACE

extern SOCKET client;

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
	Proverava da li na drugom serveru postoji klijent koji je povezan na red

	Argument:
		queueName -> ime reda za koji treba proveriti postojanje klijenta

	Povratna vrednost:
		true/false 

*/
bool Exist(const char* queueName)
{
	MESSAGE* message = make_message_config(map_queue_name_to_type(queueName),_DOES_EXIST_,_SERVER_,_SERVER_);
	MESSAGE_STATE state = send_message_tcp(client, *message);

	bool retval = false;

	if (state == FAULT)
	{
		printf("\nFailed to send EXIST message");
		free_message(&message);

		return false;
	}

	printf("\nSUCCESSFULLY SENT EXIST MESSAGE");
	
	state = receive_message_tcp(client, message);

	if (state == FAULT)
	{
		printf("\nFailed to receive message");
		free_message(&message);
		return false;
	}

	print_message(*message);
	free_message(&message);
	
	return true;
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



/*
	Ovo je funkcija koja predstavlja prijemni thread klijenta, thread se budi na svaku sekundu.
	Kada se probudi pokusava da primi poruku na klijentskom socketu i da je ispise na ekran
	Thread se zavrsava sa signaliziranjem FinishSignal semafora odnosno kada korisnik pritisne ENTER
*/
DWORD WINAPI RunAcceptingThread(LPVOID lpParam)
{
	MESSAGE* message = allocate_message();
	printf("\nStarted client accepting thread");
	MESSAGE_STATE state = FAULT;
	while (WaitForSingleObject(FinishSignal,1000) != WAIT_OBJECT_0)
	{
		state = receive_message_tcp(client, message);
		if (state == DISCONNECT || state == FAULT)
		{
			free_message(&message);
			while (WaitForSingleObject(socket_mutex, INFINITE) != WAIT_OBJECT_0);
			stop_client();
			ReleaseSemaphore(FinishSignal, 2, NULL);
			ReleaseMutex(socket_mutex);
			
			return 0;
		}

		EnterCriticalSection(&console_section);
		print_message(*message);
		LeaveCriticalSection(&console_section);
	
	}

	free_message(&message);

	return 0;
}


/*
	Ova funkcija predstavlja klijentski thread koji salje poruke, vreme na koje se thread budi je promenljivo, odnosno korisnik ga unosi sa tastature
	Vremenom budjenja ovog threada odredjuje se optrecenje celog sistema sto predstavlja i stress test kada je vreme budjenja malo

	Ovaj thread generise random podatak u zavisnosi od prosledjenog mu tipa
	i salje ga ka serveru kroz klijentski socket

*/
DWORD WINAPI RunSendingThread(LPVOID lpParam)
{
	TYPE type = *(TYPE*)lpParam;

	MESSAGE* message = allocate_message();
	MESSAGE_STATE state = FAULT;
	printf("\nStarted client sending thread");
	
	while (WaitForSingleObject(FinishSignal, 100) != WAIT_OBJECT_0)
	{
		void* random_data = get_random_data(type);
		message = make_message_data(random_data, type);

		while (WaitForSingleObject(socket_mutex, INFINITE) != WAIT_OBJECT_0);
		
		state = send_message_tcp(client, *message);

		if (state == FAULT)
		{
			ReleaseMutex(socket_mutex);
			ReleaseSemaphore(FinishSignal, 1, NULL);
			free_void_buffer(&random_data);
			free_message(&message);
			
			return 0;
		}

		ReleaseMutex(socket_mutex);
		free_void_buffer(&random_data);
	}

	free_message(&message);

	return 0;
}

#pragma endregion




void handle_init_error_handler()
{
	close_socket(client);
	close_winsock();
	exit(-10);
}
