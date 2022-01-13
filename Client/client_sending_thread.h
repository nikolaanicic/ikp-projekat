#ifndef _CLIENT_SENDING_THREAD
#define _CLIENT_SENDING_THREAD

#include "high_level_connection.h"
#include <Windows.h>
#include "common.h"
#include "client_config.h"

DWORD WINAPI RunSendingThread(LPVOID lpParam);


#endif
