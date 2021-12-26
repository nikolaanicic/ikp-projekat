#ifndef _COMMS_INTERFACE_H
#define _COMMS_INTERFACE_H

#include <WinSock2.h>

#define SERVER_PORT_1 (27000)
#define SERVER_PORT_2 (28000)

SOCKET Connect(const char* queueName);
bool Exist(const char* queueName);
bool Send_Message(void* message, char type);


#endif