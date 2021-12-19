#ifndef _DATA_MODEL_H
#define _DATA_MODEL_H


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



void print_buffer(char*, int);
void print_data(DATA, TYPE);
void print_data_message(DATA_MESSAGE);
void print_config_message(CONFIG_MESSAGE);
void print_inner_message(INNER_MESSAGE, char);
void print_message(MESSAGE);

char* allocate_buffer(int size);
void free_buffer(char*);

#endif
