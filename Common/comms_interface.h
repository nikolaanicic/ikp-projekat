#ifndef _COMMS_INTERFACE_H
#define _COMMS_INTERFACE_H

#include <WinSock2.h>

SOCKET Connect(const char* queueName);
bool Exist(const char* queueName);
bool Send_Message(void* message, char type);


#endif