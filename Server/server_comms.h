#ifndef _SERVER_COMMS_H
#define _SERVER_COMMS_H

#include "client_hash_array.h"
#include "winsock_helpers.h"
#include <stdio.h>
#include <Windows.h>


DWORD WINAPI load_balancer(LPVOID lpParam);
bool init_server(SOCKADDR_IN address);

#define MAX_CONNECTIONS (10)





#endif
