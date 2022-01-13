#include "server_main_thread.h"

/*
	Ova funkcija alocira dinamicki niz socketa odredjene duzine i inicijalizuje ga sa vrednoscu INVALID_SOCKET

	Povratna vrednost:
		inicijalizovan niz ili NULL

*/
SOCKET* allocate_socket_array(int length)
{
	SOCKET* socks = (SOCKET*)malloc(sizeof(SOCKET) * length);
	
	if (socks != NULL)
	{
		for (int i = 0; i < length; i++)
		{
			socks[i] = INVALID_SOCKET;
		}
	}

	return socks;
}


/*
	Ova funkcija oslobadja niz socketa

*/
void free_sockets(SOCKET** sockets)
{
	if (*sockets != NULL)
	{
		free(*sockets);
		*sockets = NULL;
	}
}


/*
	Ova funkcija prihvata klijentsku konekciju u niz konekcija 

	Argumenti:
		accepted_connections -> niz socketa u koji trebaprihvatiti konekciju
		free_slot -> slobodan slot u koji je moguce prihvatiti konekciju
		listen_socket -> socket sa kog se prihvata konekcija

*/
bool accept_client_connection(SOCKET* accepted_connections, int free_slot,SOCKET listen_socket)
{
	SOCKADDR_IN client_address;
	int size = sizeof(client_address);
	unsigned long mode = 1;

	accepted_connections[free_slot] = accept(listen_socket, (SOCKADDR*)&client_address, &size);

	if (accepted_connections[free_slot] == INVALID_SOCKET)
	{
		printf("\nFailed to accept client connection");
		return false;
	}
	else if (!set_non_blocking_mode(accepted_connections[free_slot]))
	{
		printf("\nFailed to set non blocking mode to client socket");
		return false;
	}
	return true;
}


/*
	Ova funkcija pronalazi slobodno mesto u nizu prihvacenih socketa
	Slobodno mesto je oznaceno INVALID_SOCKET konstantom

	Povratna vrednost:
		indeks slobodnog slota ili -1 ako nema slobodni mesta

*/
int get_free_client_slot(SOCKET accepted_connections[], int length)
{
	for (int i = 0; i < length; i++)
	{
		if (accepted_connections[i] == INVALID_SOCKET)
			return i;
	}

	return -1;
}


/*
	Ova funkcija postavlja niz soketa u skup fajl deskriptora nad kojim ce biti uradjen select

	Argumenti:
		fds -> pokazivac na skup fajl deskriptora
		accepted_connections -> niz soketa koje treba postaviti u skup
		len -> duzina niza socketa

*/
void set_sockets(fd_set* fds, SOCKET accepted_connections[],int len)
{
	for (int i = 0; i < len; i++)
	{
		if (accepted_connections[i] != INVALID_SOCKET)
		{
			FD_SET(accepted_connections[i], fds);
		}
	}
}


/*
	Ova funkcija proverava da li je poruka koja se obradjuje tipa config i da li je komadanda u njih CONNECT
	
	Povratna vrednost:
	true/false

*/
bool is_connect_message(MESSAGE* message)
{
	if (message == NULL)
		return false;
	return message->has_data == _CONFIG_ && message->data.config.command == _CONNECT_;
}



/*
	Ova funkcija pokusava da poveze klijenta na zeljeni red
	
	Ako je red slobodan upisuje accepted_socket u client_hash_array
	Ako je red zauzet salje shutdown signal klijentu

	Argumenti:
		type -> tip reda na koji treba povezati klijenta ako je red slobodan
		accepted_socket -> socket kroz koji je ostvarena veza sa klijentom

	Povratna vrednost:
		true/false
*/
bool try_connect_client_to_queue(TYPE type,SOCKET accepted_socket)
{
	bool retval = false;

	int index = map_type_to_index(type);

	while (WaitForSingleObject(client_hash_array[index].client_mutex, INFINITE) != WAIT_OBJECT_0);


	if (client_hash_array[index].socket == INVALID_SOCKET)
	{
		client_hash_array[index].socket = accepted_socket;
		printf("\nClient connected to queue: %s", map_type_to_queue_name(type));
		retval = true;
	}
	else
	{
		printf("\nClient connection to queue <%s> is rejected", map_type_to_queue_name(type));
		shutdown(accepted_socket, SD_BOTH);
	}

	ReleaseMutex(client_hash_array[index].client_mutex);
	return retval;

}

/*
	Ova funkcija odgovara klijentu ako je uspesno povezan na red

	Argumenti:
		client -> klijentski socket
		message -> poruka koja se salje klijentu

	Povratna vrednost:
		true/false

*/
bool respond_client_connected(SOCKET client, MESSAGE* message)
{

	MESSAGE_STATE state = send_message_tcp(client, *message);

	if (state == FAULT)
	{
		printf("\nFailed to connect client");
		close_socket(client);
		return false;
	}
	else if(state == SUCCESS)
	{
		printf("\nClient is connected to queue:%s",message->queueName);
		return true;
	}

	return true;
}


/*
	Ova funkcija ubacuje poruku na glavni serverski red

	Ceka na empty semaforu za glavni serverski red
	Kada dobije mogucnost pristupa ubacuje poruku na red

	Argumenti:
		message -> poruka koju treba ubaciti na serverski red

*/
void push_message_to_server_queue(MESSAGE* message)
{
	while (WaitForSingleObject(MainEmptySemaphore, INFINITE) != WAIT_OBJECT_0);
	push(&main_queue, get_new_node(message));
	ReleaseSemaphore(MainFullSemaphore, 1, NULL);

}


/*
	Ova funkcija sluzi kao wrapper za try_connect_client i respond_client_connected

	Pokusava da poveze klijenta na red i ako ga uspesno poveze odgovara mu da je uspesno povezen
	U suprutnom unutar funkcija try_connect_client veza sa klijentom se zatvara

*/
bool connect_message_handler(MESSAGE* message, TYPE type, SOCKET client_socket)
{
	bool retval = false;
	if (try_connect_client_to_queue(type, client_socket)) {
		retval = respond_client_connected(client_socket, message);
		
	}
	return retval;
}


/*
	Ova funkcija proverava dogadjaje na nizu socketa u kom se cuvaju veze sa thradovima na bratskom serveru
	
	Jedini dogadjaj koji se moze destiti je da treba primiti poruku od threada sa drugog servera i staviti je na glavni red odnosno
	obraditi je kroz isti pipeline kao i klijentsku poruku

*/
void check_brother_server_sockets_events(SOCKET brother_server_sockets[],int length,FD_SET* fds,MESSAGE* buffer)
{
	for (int i = 0; i < length; i++)
	{
		if (brother_server_sockets[i] != INVALID_SOCKET && FD_ISSET(brother_server_sockets[i], fds))
		{
			MESSAGE_STATE state = receive_message_tcp(brother_server_sockets[i], buffer);
			
			if (state == SUCCESS)
			{
				push_message_to_server_queue(buffer);
			}
		}
	}
}

/*
	Ova funkcija obradjuje dogadjaje na klijentskim socketima

	Dogadjaji koji se mogu desiti na klijenskom socketu vezani su samo za prijem poruke
	ili za prekid konekcije

	Ako je prosledjena connect poruka, proverava se da li je red dostupan i u skladu sa tim klijent biva dalje procesiran
	Ako je primljena poruka podataka ili neka druga komanda ona se samo prosledjuje na glavni red servera

	Ako je primljen shutdown signal klijent biva diskonektovan sa reda na serveru

*/
void check_client_sockets_events(SOCKET** client_sockets,int length, FD_SET* fds,MESSAGE* buffer)
{
	for (int i = 0; i < length; i++)
	{
		if ((*client_sockets)[i] != INVALID_SOCKET && FD_ISSET((*client_sockets)[i], fds))
		{
			MESSAGE_STATE state = receive_message_tcp((*client_sockets)[i], buffer);
			
			if (state == SUCCESS)
			{
				if (is_connect_message(buffer))
				{
					TYPE type = map_queue_name_to_type(buffer->queueName);
					if (!connect_message_handler(buffer, type, (*client_sockets)[i]))
					{
						(*client_sockets)[i] = INVALID_SOCKET;
					}
				}
				else
				{
					push_message_to_server_queue(buffer);
				}
			}
			else if (state == DISCONNECT || state == FAULT)
			{
				int index= map_type_to_index(map_queue_name_to_type(buffer->queueName));
				(*client_sockets)[i] = INVALID_SOCKET;
				while (WaitForSingleObject(client_hash_array[index].client_mutex, INFINITE) != WAIT_OBJECT_0);
				client_hash_array[index].socket = INVALID_SOCKET;
				ReleaseMutex(client_hash_array[index].client_mutex);
			}
		}
	}
}


/*
	Ova metoda ceka da se svi threadovi sa bratskog servera povezu na glavni thread ovog servera
	Odnosno dok nisu pokrenuta oba servera nemoguce se komunicirati izmedju servera i klijenta
*/
void accept_brother_connections(SOCKET listen_socket, SOCKET* brother_server_sockets)
{
	FD_SET set;
	int next_free = -1;
	struct timeval time;
	time.tv_usec = 1;

	while((next_free = get_free_client_slot(brother_server_sockets,MAX_CONNECTIONS / 2)) != -1)
	{
		FD_ZERO(&set);
		FD_SET(listen_socket, &set);

		int iRes = select(0, &set, NULL, NULL, &time);

		if (iRes > 0)
		{
			accept_client_connection(brother_server_sockets, next_free, listen_socket);
		}
	}

	printf("\nACCEPTED ALL BROTHER SERVER CONNECTIONS");
}



/*
	Ova funkcija predstavlja glavni thread servera

	Ovaj thread prima zahteve za konekcijom, klijentske poruke i poruke sa bratskog servera

	Zahteve za konekcijom na red obradjuje i povezuje ili odbija klijenta
	Sve ostale zahteve i poruke prosledjuje na glavni serverski red i oni dalje bivaju obradjeni threadom zaduzenim
	za konkretan red

*/

DWORD WINAPI load_balancer(LPVOID lpParam)
{

	SOCKET* client_sockets = allocate_socket_array(MAX_CONNECTIONS / 2);
	SOCKET* brother_server_sockets = allocate_socket_array(MAX_CONNECTIONS / 2);

	int next_free = 0;
	FD_SET readfds;


	struct timeval time;
	time.tv_usec = 1;
	MESSAGE* message = allocate_message();
	accept_brother_connections(listen_socket, brother_server_sockets);

	while (WaitForSingleObject(FinishSignal, 1) != WAIT_OBJECT_0)
	{
		FD_ZERO(&readfds);
		next_free = get_free_client_slot(client_sockets, MAX_CONNECTIONS / 2);

		if (next_free != -1)
		{
			FD_SET(listen_socket, &readfds);
		}

		set_sockets(&readfds, client_sockets, MAX_CONNECTIONS / 2);
		set_sockets(&readfds, brother_server_sockets, MAX_CONNECTIONS / 2);

		int select_result = select(0, &readfds, NULL, NULL, &time);

		if (select_result == SOCKET_ERROR)
		{
			print_last_winsock_error();
		}
		else if (select_result > 0)
		{
			if (FD_ISSET(listen_socket, &readfds))
			{
				accept_client_connection(client_sockets, next_free,listen_socket);
			}

			check_client_sockets_events(&client_sockets,MAX_CONNECTIONS / 2, &readfds, message);
			check_brother_server_sockets_events(brother_server_sockets, MAX_CONNECTIONS / 2, &readfds, message);
		}
	}

	free_message(&message);
	free_sockets(&client_sockets);
	free_sockets(&brother_server_sockets);

	return 0;
}
