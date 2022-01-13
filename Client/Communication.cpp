#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")

#include "Communication.h"
#include <stdlib.h>

extern HANDLE FinishSignal;
extern CRITICAL_SECTION console_section;
extern HANDLE socket_mutex;


#define STRESS_TEST_PERIOD (10)

enum WORK_MODE { _MENU_=1, _STRESS_TEST_=2 };

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

DWORD get_waiting_period(bool stress_test = false)
{
	DWORD retval = -1;

	if (!stress_test)
	{
		do
		{
			EnterCriticalSection(&console_section);
			printf("\nUnesite period [ >= 0 ] ms na koji ce klijent slati poruke:");
			scanf_s("%d", &retval);
			LeaveCriticalSection(&console_section);


		}while(retval <= 0);
	}
	else
	{
		retval = STRESS_TEST_PERIOD;
	}

	return retval;
}



WORK_MODE get_work_mode()
{
	int work_mode = -1;

	do
	{
		EnterCriticalSection(&console_section);
		printf("\n1.Menu");
		printf("\n2.Stress test");
		printf("\n");
		LeaveCriticalSection(&console_section);
		scanf_s("%d", &work_mode);
		
		if (work_mode != (int)_MENU_ && work_mode != _STRESS_TEST_)
		{
			work_mode = -1;
		}

	} while (work_mode == -1);

	return (WORK_MODE)work_mode;
}



int get_menu_option()
{

	int option = -1;

	do
	{
		EnterCriticalSection(&console_section);
		printf("\n1.Send random message");
		printf("\n2.Send does exist message");
		LeaveCriticalSection(&console_section);
		scanf_s("%d", &option);

		if (option != 1 && option != 2)
		{
			option = -1;
		}

	} while (option == -1);

	return option;
}



int stress_test_mode(TYPE type, DWORD period)
{
	MESSAGE* message = NULL;
	MESSAGE_STATE state;
	while (WaitForSingleObject(FinishSignal, period) != WAIT_OBJECT_0)
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


MESSAGE* get_optioned_message(int option,TYPE type)
{
	MESSAGE* message = NULL;
	void* random_data = NULL;
	switch (option)
	{
	case 1:
		random_data = get_random_data(type);
		message = make_message_data(random_data, type);
		break;
	case 2:
		message = make_message_config(type, _DOES_EXIST_, _CLIENT_, _SERVER_);
		break;

	case 3:
		ReleaseSemaphore(FinishSignal, 3, NULL);
		free_void_buffer(&random_data);
		return NULL;
	}

	
	free_void_buffer(&random_data);
	return message;
}

int menu_mode(TYPE type)
{
	MESSAGE* message = NULL;
	MESSAGE_STATE state;
	while (WaitForSingleObject(FinishSignal, 500) != WAIT_OBJECT_0)
	{
		message = get_optioned_message(get_menu_option(), type);

		while (WaitForSingleObject(socket_mutex, INFINITE) != WAIT_OBJECT_0);

		state = send_message_tcp(client, *message);

		if (state == FAULT)
		{
			ReleaseMutex(socket_mutex);
			ReleaseSemaphore(FinishSignal, 1, NULL);
			free_message(&message);

			return 0;
		}
		ReleaseMutex(socket_mutex);
	}
	free_message(&message);

	return 0;
}
/*
	Ova funkcija predstavlja klijentski thread koji salje poruke, vreme na koje se thread budi je promenljivo, odnosno korisnik ga unosi sa tastature
	Vremenom budjenja ovog threada odredjuje se opterecenje celog sistema sto predstavlja i stress test kada je vreme budjenja malo

	Moguci modovi rada su MENU, STRESS_TEST

	U MENU modu rada korisik kontrolise rad klijentskog programa, kada se odabere STRESS_TEST opcija
	klijent automatski generise i salje poruke u jako malom razmaku

	Ovaj thread generise random podatak u zavisnosi od prosledjenog mu tipa
	i salje ga ka serveru kroz klijentski socket

*/
DWORD WINAPI RunSendingThread(LPVOID lpParam)
{
	TYPE type = *(TYPE*)lpParam;

	MESSAGE_STATE state = FAULT;

	printf("\nStarted client sending thread");

	WORK_MODE mode = get_work_mode();

	if (mode == _STRESS_TEST_)
	{
		DWORD period = get_waiting_period(true);
		return stress_test_mode(type, period);
	}
	else if (mode == _MENU_)
	{
		return menu_mode(type);
	}

	return 0;
}

#pragma endregion




void handle_init_error_handler()
{
	close_socket(client);
	close_winsock();
	exit(-10);
}
