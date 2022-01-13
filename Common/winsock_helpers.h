#ifndef _WINSOCK_HELPERS_
#define _WINSOCK_HELPERS

#include <WinSock2.h>
#include <stdio.h>
#define SERVER_PORT_1 (27000)
#define SERVER_PORT_2 (28000)

void print_last_winsock_error();
void init_winsock(WORD);
void close_winsock();
void handle_winsock_error(void (*)());
SOCKET init_socket();
void handle_socket_init_error();

bool close_socket(SOCKET);

bool connect_to_address(SOCKET, SOCKADDR_IN);

void populateAddressV4(SOCKADDR_IN*, const char*, short);

void print_address(SOCKADDR_IN address);

unsigned short get_server_port();


#endif
