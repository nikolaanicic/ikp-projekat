#include "synchronization.h"



/*
	Ova funkcija inicijalizuje anonimni semafor

	Argumenti:
		initial_count -> pocetna vrednost semafora
		max_count -> maksimalna vrednost koju semafor moze drzati

	Povratna vrednost:
		ako je dobro inicijalizovan semafor vraca HANDLE
		ako dodje do greska ubija proces jer server ne moze pravilno raditi bez semafora

*/
HANDLE init_semaphore(int initial_count, int max_count)
{
	HANDLE semaphore = CreateSemaphore(0, initial_count, max_count, NULL);

	if (!semaphore)
	{
		printf("\nFailed to initialize semaphore");
		exit(-19);
	}

	return semaphore;
}

/*
	Ova funkcija zatvara proslednjeni handle

*/

void safe_close_handle(HANDLE handle)
{
	if (handle)
	{
		CloseHandle(handle);
	}
}

/*
	Ova funkcija inicijalizuje anonimni mutex koji JE u vlasnistvu one niti koja je pozvala funkciju

	Povratna vrednost:
		HANDLE na mutex ili NULL
*/


HANDLE init_mutex()
{
	return CreateMutex(NULL, true, NULL);
}


/*
	Ova funkcija inicijalizuje anonimni mutex koji je u NIJE vlasnistvu one niti koja je pozvala funkciju

	Povratna vrednost:
		HANDLE na mutex ili NULL
*/
HANDLE init_not_owned_mutex()
{
	return CreateMutex(NULL, false, NULL);
}