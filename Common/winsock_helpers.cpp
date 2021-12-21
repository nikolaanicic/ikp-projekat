#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "winsock_helpers.h"

#pragma comment(lib, "Ws2_32.lib")



void print_address(SOCKADDR_IN address)
{
	printf("\nADDRESS:<%s,%d>", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
}


void print_last_winsock_error()
{
	printf("\nError in winsock initialization %d", WSAGetLastError());

}

void init_winsock(WORD version)
{
	WSADATA wsa;

	if (WSAStartup(version, &wsa) != 0)
	{
		print_last_winsock_error();
		exit(-1);
	}
}

void close_winsock()
{
	if (WSACleanup() != 0)
	{
		printf("\nError in winsock cleanup");
		exit(-2);
	}
}

void handle_winsock_error(void (*error_handler)())
{
	(*error_handler)();
}

SOCKET init_socket()
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == SOCKET_ERROR)
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


void connect_to_address(SOCKET socket, SOCKADDR_IN address, void (*connect_error_handler)())
{
	int addressSize = sizeof(address);
	int iRes = connect(socket, (SOCKADDR*)&address, addressSize);

	if (iRes != 0)
	{
		handle_winsock_error(connect_error_handler);
	}
}

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
