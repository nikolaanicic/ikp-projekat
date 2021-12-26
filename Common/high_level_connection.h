#ifndef _HIGH_LEVEL_CONNECTION
#define _HIGH_LEVEL_CONNECTION

#include <WinSock2.h>
#include "data_model.h"
#include "winsock_helpers.h"

MESSAGE_STATE send_message_tcp(SOCKET, MESSAGE);
MESSAGE_STATE receive_message_tcp(SOCKET, MESSAGE*);

#endif
