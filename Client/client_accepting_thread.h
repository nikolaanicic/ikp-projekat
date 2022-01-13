#ifndef _CLIENT_ACCEPTING_THREAD_H
#define _CLIENT_ACCPETING_THREAD_H

#include "high_level_connection.h"
#include <Windows.h>
#include "common.h"

DWORD WINAPI RunAcceptingThread(LPVOID lpParam);

#endif