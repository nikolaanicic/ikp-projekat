#ifndef _COMMS_H
#define _COMMS_H

#include "framework.h"
#include <string.h>
#include <stdio.h>
#include <Windows.h>


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT (10000)
#define BUFFER_SIZE (512)


void safe_close_handle(HANDLE);


SOCKET Connect(const char* queueName);
bool Exist(const char* queueName);
bool Send_Message(void* message, char type);


DWORD WINAPI RunAcceptingThread(LPVOID lpParam);
DWORD WINAPI RunSendingThread(LPVOID lpParam);


void handle_init_error_handler();


#endif

