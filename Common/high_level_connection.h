#ifndef _HIGH_LEVEL_CONNECTION
#define _HIGH_LEVEL_CONNECTION

#include <WinSock2.h>
#include "data_model.h"

MESSAGE_STATE send_message_tcp(SOCKET, MESSAGE);
MESSAGE_STATE receive_message_tcp(SOCKET, MESSAGE*);
MESSAGE* allocate_message();
void free_message(MESSAGE*);


bool serialize_message(void*, MESSAGE);
bool deserialize_message(void*, MESSAGE*);


MESSAGE* make_message_config(TYPE queueType, COMMAND command, LOCATION origin, LOCATION destination);
MESSAGE* make_message_data(void* data, TYPE type);


TYPE map_queue_name_to_type(const char* queue_name);
const char* map_type_to_queue_name(TYPE type);


#endif
