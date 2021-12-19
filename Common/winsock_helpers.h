#ifndef _WINSOCK_HELPERS_
#define _WINSOCK_HELPERS

#include <WinSock2.h>

void print_last_winsock_error();
void init_winsock(WORD);
void close_winsock();
void handle_winsock_error(void (*)());
SOCKET init_socket();
void handle_socket_init_error();

bool close_socket(SOCKET);

void connect_to_address(SOCKET, SOCKADDR_IN, void (*)());

void populateAddressV4(SOCKADDR_IN*, const char*, short);

void print_address(SOCKADDR_IN address);

#endif
