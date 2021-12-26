#define _CRT_SECURE_NO_WARNINGS
#include "data_model.h"

void print_buffer(char* buffer, int buffer_len)
{
	if (buffer == NULL || buffer_len <= 0)
		return;

	for (int i = 0; i < buffer_len; i++)
	{
		printf("%c", buffer[i]);
	}
}

void print_data(DATA data, TYPE type)
{
	printf("\nDATA:");
	switch (type)
	{
	case _CHAR_: printf("%c", data.c); break;

	case _INT_:	printf("%d", data.i); break;
	case _SHORT_: printf("%d", data.s); break;
	case _FLOAT_: printf("%f", data.f); break;
	case _DOUBLE_: printf("%.2lf", data.d);  break;
	}
}

void print_command(COMMAND command)
{
	printf("\nCOMMAND:");
	switch (command)
	{
	case _CONNECT_: printf("CONNECT"); break;
	case _DOES_EXIST_: printf("DOES EXIST"); break;
	case _EXISTS_: printf("EXISTS"); break;
	case _MESSAGE_: printf("MESSAGE"); break;
	case _NOT_EXISTS_: printf("NOT EXISTS"); break;
	}
}

void print_location(LOCATION location)
{
	switch (location)
	{
	case _CLIENT_: printf("CLIENT"); break;
	case _SERVER_: printf("SERVER"); break;
	}
}

void print_type(TYPE type)
{
	printf("\nTYPE:");
	switch (type)
	{
	case _INT_: printf("INT"); break;
	case _CHAR_: printf("CHAR"); break;
	case _SHORT_: printf("SHORT"); break;
	case _FLOAT_: printf("FLOAT"); break;
	case _DOUBLE_: printf("DOUBLE"); break;
	}
}

void print_data_message(DATA_MESSAGE message)
{
	print_data(message.data, message.type);
}

void print_config_message(CONFIG_MESSAGE message)
{
	print_command(message.command);
}

void print_inner_message(INNER_MESSAGE message, FILLED_STRUCTURE has_data)
{
	switch (has_data)
	{
		case _CONFIG_: print_config_message(message.config); break;
		case _DATA_: print_data_message(message.data); break;
	}
}

void print_message(MESSAGE message)
{
	printf("\n*********");
	printf("\nORIGIN:"); print_location(message.origin);
	printf("\nDESTINATION:"); print_location(message.destination);
	printf("\nQUEUE NAME:%s", message.queueName);

	print_inner_message(message.data, message.has_data);
	printf("\n*********");

}

void handle_no_memory_buffer()
{
	printf("\nNo memory to allocate new message buffer");
}

char* allocate_buffer(int size)
{
	char* buffer = (char*)malloc(size);

	if (buffer == NULL)
	{
		handle_no_memory_buffer();
	}

	return buffer;
}

void free_buffer(char* buffer)
{
	if (buffer != NULL)
	{
		free(buffer);
		buffer = NULL;
	}
}

MESSAGE* allocate_message()
{
	MESSAGE* message = (MESSAGE*)malloc(sizeof(MESSAGE));
	if (message == NULL)
	{
		printf("\nFailed to allocate message buffer");

	}

	return message;
}

const char* map_type_to_queue_name(TYPE type)
{
	switch (type)
	{

	case _INT_: return "int";
	case _CHAR_: return "char";
	case _SHORT_: return "short";
	case _FLOAT_: return "float";
	case _DOUBLE_: return "double";
	}
}

bool serialize_message(void* buffer, MESSAGE message)
{
	if (buffer == NULL)
		return false;

	memcpy(buffer, (char*)&message, sizeof(MESSAGE));

	return true;
}

bool deserialize_message(void* buffer, MESSAGE* message)
{
	if (buffer == NULL || message == NULL)
		return false;

	memcpy(message, buffer, sizeof(MESSAGE));
	return true;
}

bool populate_data(DATA* d, TYPE type, void* data)
{
	if (d == NULL || data == NULL)
		return false;

	switch (type)
	{
	case _CHAR_:
	{
		char c = *(char*)data;
		d->c = c;
		break;
	}

	case _INT_:
	{
		int i = *(int*)data;
		d->i = i;
		break;

	}
	case _SHORT_:
	{
		short s = *(short*)data;
		d->s = s;
		break;
	}
	case _FLOAT_:
	{
		float f = *(float*)data;
		d->f = f;
		break;
	}
	case _DOUBLE_:
	{
		double dou = *(double*)data;
		d->d = dou;
		break;
	}
	default:
		return false;
	}

	return true;
}

bool populate_config_message(CONFIG_MESSAGE* message, COMMAND command)
{
	if (message == NULL)
		return false;

	message->command = command;


	return true;
}

bool populate_data_message(DATA_MESSAGE* message, TYPE type, void* data)
{
	if (message == NULL || data == NULL)
		return false;

	message->type = type;
	bool retval = populate_data(&(message->data), type, data);

	return retval;
}

bool populate_inner_message(INNER_MESSAGE* message, void* data, FILLED_STRUCTURE has_data)
{
	if (message == NULL || data == NULL)
		return false;

	switch (has_data)
	{
	case _CONFIG_:
	{
		memcpy(&(message->config), data, sizeof(CONFIG_MESSAGE));
		break;
	}
	case _DATA_:
	{
		memcpy(&(message->data), data, sizeof(DATA_MESSAGE));
		break;
	}
	}

	return true;
}

TYPE map_queue_name_to_type(const char* queue_name)
{
	char c = queue_name[0];

	return c == 's' ? _SHORT_ : (TYPE)c;

}

MESSAGE* make_message_data(void* data, TYPE type)
{
	MESSAGE* message = allocate_message();

	DATA_MESSAGE data_message;
	populate_data_message(&data_message, type, data);
	populate_inner_message(&(message->data), (void*)&data_message, _DATA_);

	message->destination = _CLIENT_;
	message->has_data = _DATA_;
	message->origin = _CLIENT_;


	strcpy(message->queueName, map_type_to_queue_name(type));
	return message;
}

MESSAGE* make_message_config(TYPE queueName, COMMAND command, LOCATION origin, LOCATION destination)
{

	MESSAGE* message = allocate_message();
	CONFIG_MESSAGE config_message;
	populate_config_message(&config_message, command);
	populate_inner_message(&(message->data), (void*)(&config_message), _CONFIG_);

	message->has_data = _CONFIG_;
	message->destination = destination;
	message->origin = origin;

	strcpy(message->queueName, map_type_to_queue_name(queueName));


	return message;
}

void free_message(MESSAGE* message)
{
	if (message != NULL)
	{
		free(message);
		message = NULL;
	}
}
