#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "winsock_helpers.h"

#pragma comment(lib, "Ws2_32.lib")


/*
	Ova funkcija ispisuje SOCKADDR_IN strukturu
*/
void print_address(SOCKADDR_IN address)
{
	printf("\nADDRESS:<%s,%d>", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
}


/*
	Ova funkcija ispisuje poslednju gresku koja se desila u winsock biblioteci

*/
void print_last_winsock_error()
{
	printf("\nError in winsock %d", WSAGetLastError());

}


/*
	Inicijalizuje winsock
	Nema povratnu vrednost
	Ako dodje do greske ubija proces

*/
void init_winsock(WORD version)
{
	WSADATA wsa;

	if (WSAStartup(version, &wsa) != 0)
	{
		print_last_winsock_error();
		exit(-1);
	}
}


/*
	Ova funkcija radi cleanup winsock biblioteke ako dodje do greske ubija proces

*/
void close_winsock()
{
	if (WSACleanup() != 0)
	{
		printf("\nError in winsock cleanup");
		exit(-2);
	}
}


/*
	Prima pokazivac na funkciju koji hendla gresku na prikladan nacin u zavisnosti od toga u kom se stadijumu program nalazi
	Plan je bio da svaki deo podesavanja ima svoj error handler pa da se pozivaju kroz ovaj wrapper, svakako moze sluziti za unapredjenja i sredjivanje koda

*/
void handle_winsock_error(void (*error_handler)())
{
	(*error_handler)();
}


/*
	Ova funkcija inicijalizuje tcp socket,

	Povratna vrednost je socket ako je uspesno kreiran

	Ako dodje do greske ili nije moguce zauzeti socket ubija proces

*/
SOCKET init_socket()
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET)
	{
		handle_winsock_error(&handle_socket_init_error);
		exit(-3);
	}

	return sock;
}

void handle_socket_close_error()
{
	print_last_winsock_error();
	close_winsock();
}

void handle_socket_init_error()
{
	print_last_winsock_error();
	close_winsock();
}



/*
	Ova funkcia zatvara socket

	Povratna vrednost pokazuje ispravnost zatvaranja socketa
*/
bool close_socket(SOCKET socket)
{
	bool retval = true;

	if (closesocket(socket) != 0)
	{
		handle_winsock_error(&handle_socket_close_error);
		retval = false;
	}

	return retval;
}



/*
	Ova funkcija povezuje socket na zeljenu adresu

	Argumenti:
		socket  -> socket kroz koji treba ostvariti vezu sa adresom
		address -> adresa na koju treba povezati socket

	Povratna vrednost:
		true/false u zavisnosti od toga da li je konekcija uspesna ili nije

*/
bool connect_to_address(SOCKET socket, SOCKADDR_IN address)
{
	int addressSize = sizeof(address);
	int iRes = connect(socket, (SOCKADDR*)&address, addressSize);

	if (iRes != 0)
	{
		return false;
	}

	return true;
}


/*
	Ova funkcija popunjava SOCKADDR_IN strukturu sa zeljenom V4 ip adresom i portom

	Nema povratnu vrednost
*/
void populateAddressV4(SOCKADDR_IN* address_struct, const char* ip, short port)
{
	if (address_struct == NULL)
		return;
	else if (ip == NULL)
		return;

	address_struct->sin_addr.S_un.S_addr = inet_addr(ip);
	address_struct->sin_port = htons(port);
	address_struct->sin_family = AF_INET;
}


/*
	Ova funkcija uzima od korisnika podatak sa kojim serverom je potrebno raditi
	Koristi se u kontekstu pokretanja servera kao i povezivanja klijenata na servere

	Povratna vrednost:
		SERVER_PORT_1 ili SERVER_PORT_2

*/
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