#ifndef _SERVER_CONFIG_H
#define _SERVER_CONFIG_H

#include "winsock_helpers.h"
#include "server_common.h"
#include "queue.h"
#include "client_hash_array.h"
#include "server_main_thread.h"
#include "comms_interface.h"
#include "synchronization.h"
#include "request_worker.h"

#define MAX_CONNECTIONS (10)
#define WORKER_NUM (5)
// pet klijentata i pet worker threadova sa drugog servera


SOCKADDR_IN server_starter();
void safe_close_server();
void allocate_resources();




#endif
