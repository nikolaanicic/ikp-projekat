#ifndef _SERVER_COMMS_H
#define _SERVER_COMMS_H

#include "framework.h"
#include <stdio.h>
#include <Windows.h>
#include "server_data_model.h"
#include "server_data_model.h"


DWORD WINAPI load_balancer(LPVOID lpParam);
bool init_server(SOCKADDR_IN address);

#define MAX_CONNECTIONS (10)





#endif
