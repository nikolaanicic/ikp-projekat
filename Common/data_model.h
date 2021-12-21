#ifndef _DATA_MODEL_H
#define _DATA_MODEL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE (512)

enum TYPE { _CHAR_ = 99, _DOUBLE_ = 100, _FLOAT_ = 102, _SHORT_ = 104, _INT_ = 105 };
enum COMMAND { _CONNECT_ = 1, _DOES_EXIST_ = 2, _MESSAGE_ = 3, _EXISTS_ = 4, _NOT_EXISTS_ = 5 };
enum LOCATION { _SERVER_ = 1, _CLIENT_ = 2 };
enum MESSAGE_STATE { SUCCESS = 0, FAULT = 1 };


//recnik komadni
// C - connect
// E - exists
// M - message
// A - accept client
// n - not exists client


typedef union __data__
{
	char c;
	int i;
	double d;
	float f;
	short s;
}DATA;

typedef struct _data_message_
{
	DATA data;
	TYPE type;
}DATA_MESSAGE;

typedef struct _config_message_
{
	COMMAND command;

}CONFIG_MESSAGE;



typedef union _inner_message_
{
	CONFIG_MESSAGE config;
	DATA_MESSAGE data;

}INNER_MESSAGE;


typedef struct _message_
{
	LOCATION destination;
	LOCATION origin;
	char has_data;
	char queueName[7];
	INNER_MESSAGE data;
}MESSAGE;


void print_message(MESSAGE);

char* allocate_buffer(int size);
void free_buffer(char*);

MESSAGE* make_message_config(TYPE queueType, COMMAND command, LOCATION origin, LOCATION destination);
MESSAGE* make_message_data(void* data, TYPE type);

TYPE map_queue_name_to_type(const char* queue_name);
const char* map_type_to_queue_name(TYPE type);

void free_message(MESSAGE*);
MESSAGE* allocate_message();
bool serialize_message(void* buffer, MESSAGE message);
bool deserialize_message(void* buffer, MESSAGE* message);

#endif
