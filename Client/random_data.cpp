#include "random_data.h"


char* generateRandomData()
{
	int l = rand();
	int r = rand();

	char* buffer = allocate_buffer(RAND_BUFFER_SIZE);

	memcpy(buffer, &l, sizeof(int));
	memcpy(&(buffer[1]), &r, sizeof(int));

	return buffer;
}


void free_void_buffer(void* buffer)
{
	if (buffer != NULL)
	{
		free(buffer);
		buffer = NULL;
	}
}

int get_random_int(char* buffer)
{
	return ((int*)buffer)[rand() % (RAND_BUFFER_SIZE / sizeof(int))];
}

char get_random_char(char* buffer)
{
	return buffer[rand() % RAND_BUFFER_SIZE];
}

short get_random_short(char* buffer)
{
	return ((short*)buffer)[rand() % (RAND_BUFFER_SIZE / sizeof(short))];
}

float get_random_float(char* buffer)
{
	return ((float*)buffer)[rand() % (RAND_BUFFER_SIZE / sizeof(float))];
}

double get_random_double(char* buffer)
{
	return *((double*)buffer);
}


void* allocate_void_buffer(int size)
{
	void* buffer = malloc(size);

	if (buffer == NULL)
	{
		printf("\nFailed to allocate buffer");
	}

	return buffer;
}



void* get_random_data(TYPE type)
{
	char* random_buffer = generateRandomData();
	void* data_buffer = NULL;
	switch (type)
	{
	case _CHAR_:
	{
		data_buffer = allocate_void_buffer(sizeof(char));
		char c = get_random_char(random_buffer);
		memcpy(data_buffer, &c, sizeof(char));
		break;
	}
	case _INT_:
	{
		data_buffer = allocate_void_buffer(sizeof(int));
		int i = get_random_int(random_buffer);
		memcpy(data_buffer, &i, sizeof(int));
		break;
	}
	case _SHORT_:
	{
		data_buffer = allocate_void_buffer(sizeof(short));
		short s = get_random_short(random_buffer);
		memcpy(data_buffer, &s, sizeof(short));
		break;
	}
	case _FLOAT_:
	{
		data_buffer = allocate_void_buffer(sizeof(float));
		float f = get_random_float(random_buffer);
		memcpy(data_buffer, &f, sizeof(float));
		break;
	}
	case _DOUBLE_:
	{
		data_buffer = allocate_void_buffer(sizeof(double));
		double d = get_random_float(random_buffer);
		memcpy(data_buffer, &d, sizeof(double));
		break;
	}
	}

	free_buffer(random_buffer);
	return data_buffer;
}