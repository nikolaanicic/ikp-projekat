#include "client_accepting_thread.h"

/*
	Ovo je funkcija koja predstavlja prijemni thread klijenta, thread se budi na svaku sekundu.
	Kada se probudi pokusava da primi poruku na klijentskom socketu i da je ispise na ekran
	Thread se zavrsava sa signaliziranjem FinishSignal semafora odnosno kada korisnik pritisne ENTER
*/
DWORD WINAPI RunAcceptingThread(LPVOID lpParam)
{
	MESSAGE* message = allocate_message();
	printf("\nStarted client accepting thread");
	MESSAGE_STATE state = FAULT;
	while (WaitForSingleObject(FinishSignal, 1000) != WAIT_OBJECT_0)
	{
		state = receive_message_tcp(client, message);
		if (state == DISCONNECT || state == FAULT)
		{
			free_message(&message);
			while (WaitForSingleObject(socket_mutex, INFINITE) != WAIT_OBJECT_0);
			stop_client();
			ReleaseSemaphore(FinishSignal, 2, NULL);
			ReleaseMutex(socket_mutex);

			return 0;
		}

		EnterCriticalSection(&console_section);
		print_message(*message);
		LeaveCriticalSection(&console_section);

	}

	free_message(&message);

	return 0;
}