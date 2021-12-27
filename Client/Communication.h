#ifndef _COMMS_H
#define _COMMS_H

#include "comms_interface.h"
#include "winsock_helpers.h"
#include "high_level_connection.h"
#include "data_model.h"
#include <string.h>
#include <stdio.h>
#include "synchronization.h"
#include "random_data.h"

#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE (512)


DWORD WINAPI RunAcceptingThread(LPVOID lpParam);
DWORD WINAPI RunSendingThread(LPVOID lpParam);


void handle_init_error_handler();
void stop_client();

#endif

