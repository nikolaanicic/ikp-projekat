#ifndef _SERVER_COMMS_H
#define _SERVER_COMMS_H

#include "comms_interface.h"
#include "client_hash_array.h"
#include "winsock_helpers.h"
#include <stdio.h>
#include "high_level_connection.h"
#include <Windows.h>
#include "queue.h"



#define MAX_CONNECTIONS (10)
// 5 klijenata i 5 worker threadova sa drugog servera


DWORD WINAPI load_balancer(LPVOID lpParam);
bool init_server(SOCKADDR_IN address);
bool stop_server();
unsigned short get_server_port();
void run_server();


#endif
