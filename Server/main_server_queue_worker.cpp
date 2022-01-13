#include "main_server_queue_worker.h"


/*
	Ova funkcija obavalja prebacivanje poruke iz glavnog serverskog reda na jedan od klijentskih redova
	Funkcija zauzima empty semaphore reda i ceka na njemu
	Kada je semaphore signaliziran funkcija radi push na red i nastavlja dalje
	
	Argumenti:
		client_index -> indeks u hes nizu koji predstavlja mapiran tip poruke
		message -> poruka koju treba proslediti na red 

*/
void put_message_to_queue(MESSAGE* message, int client_index)
{
	while (WaitForSingleObject(client_hash_array[client_index].EmptySemaphore, INFINITE) != WAIT_OBJECT_0);
	push(&queue_heads[client_index], get_new_node(message));
	ReleaseSemaphore(client_hash_array[client_index].FullSemaphore,1,NULL);
}



/*
	Ova funkcija predstavlja glavni obradjivacki thread servera

	Ovaj thread preuzima poruke sa glavnog reda i na osnovu tipa poruke prosledjuje
	ih na red koji je zaduzen za obradu datog tipa poruke

	Thread se zavrsava na FinishSignal semaphore

	Odnosno kada korisnik pritisne ENTER na serveru

*/

DWORD WINAPI PopMessageThread(LPVOID lpParam)
{
	
	HANDLE handles[2] = { FinishSignal,MainFullSemaphore };

	int wait_retval = ~WAIT_OBJECT_0;

	while (wait_retval != WAIT_OBJECT_0)
	{

		while ((wait_retval = WaitForMultipleObjects(2, handles, FALSE, INFINITE)) == WAIT_OBJECT_0 + 1)
		{
			MESSAGE message = pop(&main_queue).data;
			int index = map_type_to_index(map_queue_name_to_type(message.queueName));
			put_message_to_queue(&message, index);

			ReleaseSemaphore(MainEmptySemaphore, 1, NULL);
			wait_retval = ~(WAIT_OBJECT_0 + 1);
			break;
		}
	}


	return 0;
}