#ifndef _SERVER_CONFIG_H
#define _SERVER_CONFIG_H

#include "winsock_helpers.h"
#include "server_common.h"
#include "queue.h"
#include "client_hash_array.h"
#include "server_main_thread.h"
#include "comms_interface.h"
#include "synchronization.h"

#define MAX_CONNECTIONS (10)
// pet klijentata i pet worker threadova sa drugog servera


bool init_server(SOCKADDR_IN address);
void run_server();
void stop_server();
unsigned short get_server_port();



#endif
