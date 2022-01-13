#include "random_data.h"


/*
	Ova funkcija generise random char bafer dugacak osam bajta

*/
char* generateRandomData()
{
	int l = rand();
	int r = rand();

	char* buffer = allocate_buffer(RAND_BUFFER_SIZE);

	memcpy(buffer, &l, sizeof(int));
	memcpy(&(buffer[1]), &r, sizeof(int));

	return buffer;
}

/*
	Ova funkcija oslobadja void pokazivac na blok memorije
*/
void free_void_buffer(void** buffer)
{
	if (*buffer != NULL)
	{
		free(*buffer);
		*buffer = NULL;
	}
}


/*
	Ova funkcija iz generisanog random bafera koji se sastoji od 8 bajtova odnosno od dva inta
	uzima jedan od dva inta koji se nalaze u baferu

	Povratna vrednost:
		random generisan integer
*/
int get_random_int(char* buffer)
{
	return ((int*)buffer)[rand() % (RAND_BUFFER_SIZE / sizeof(int))];
}

/*
	Ova funkcija iz random bafera uzima jedan od charova
*/
char get_random_char(char* buffer)
{
	return buffer[rand() % RAND_BUFFER_SIZE];
}


/*
	Ova funkcija iz random bafera uzima jedan od 4 shorta
*/
short get_random_short(char* buffer)
{
	return ((short*)buffer)[rand() % (RAND_BUFFER_SIZE / sizeof(short))];
}


/*
	Ova funkcija iz random bafera uzima jedan od 2 floata
*/
float get_random_float(char* buffer)
{
	return ((float*)buffer)[rand() % (RAND_BUFFER_SIZE / sizeof(float))];
}



/*
	Ova funkcija kastuje citav random bafer u double i vraca vrednost koja se nalazi u njemu

*/
double get_random_double(char* buffer)
{
	return *((double*)buffer);
}


/*
	Ova funkcija alocira void bafera odredjene duzine

	Povratna vrednost:
		Pokazivac na blok zauzete memorije ili NULL
*/
void* allocate_void_buffer(int size)
{
	void* buffer = malloc(size);

	if (buffer == NULL)
	{
		printf("\nFailed to allocate buffer");
	}

	return buffer;
}


/*
	Ova funkcija generise random podatak zeljenog tipa i serijalizuje ga u void bafer
	Void bafer se kasnije koristi za pravljenje poruke

	Agument:
		type -> tip podatka koji treba generisati

	Povratna vrednost:
		void bafer popunjen random generisanim podatkom zeljenog tipa
	
*/
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

	free_buffer(&random_buffer);
	return data_buffer;
}