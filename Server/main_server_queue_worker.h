#ifndef _MAIN_SERVER_QUEUE_WORKER_H_
#define _MAIN_SERVER_QUEUE_WORKER_H_

#include "server_common.h"
#include <Windows.h>
#include "queue.h"

DWORD WINAPI PopMessageThread(LPVOID lpParam);

#endif