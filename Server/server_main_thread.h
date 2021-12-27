#ifndef _SERVER_MAIN_THREAD_H
#define _SERVER_MAIN_THREAD_H

#include "winsock_helpers.h"
#include "queue.h"
#include <Windows.h>
#include "server_config.h"
#include "high_level_connection.h"
#include "client_hash_array.h"
#include "server_common.h"

extern SOCKET listen_socket;

DWORD WINAPI load_balancer(LPVOID lpParam);

#endif
