#include "request_worker.h"

/*
	Ova funkcija popunjava strukturu koja je potrebna threadu koji obradjuje jedan od redova na serveru

*/

void make_worker_params(WORKER_PARAMS* output, SOCKET* client_socket, HANDLE client_mutex, TYPE queue_type,SOCKADDR_IN brother_address)
{
	output->client_mutex = client_mutex;
	output->client_socket = client_socket;
	output->queue_type = queue_type;
	memcpy(&output->brother_address, &brother_address, sizeof(SOCKADDR_IN));
}



/*
	Ova funkcija oslobadja memoriju koja je zauzeta za parametre koji su potrebni worker threadu 

*/
void free_worker_params(WORKER_PARAMS** data)
{
	if (data == NULL)
	{
		free(data);
		data = NULL;
	}
}


/*
	Ova funkcija proverava da li je prosledjena poruka tipa config i da li je komanda u njoj DOES_EXIST
	Odnosno da li je poruka koja proverava postojanje klijenta na redu

*/
bool is_exists(MESSAGE data)
{
	return data.has_data == _CONFIG_ && data.data.config.command == _DOES_EXIST_;
}



/*
	Proverava da li je tip poruka poruka sa podacima

*/
bool is_data(MESSAGE data)
{
	return data.has_data == _DATA_;
}


/*
	Ova funkcija odredjuje kome treba proslediti poruku odnosno na osnovu odredista i izvorista odredjuje koja je poruka u pitanju
	i kome je treba proslediti 


*/
tuple parse_output_location(MESSAGE* message)
{
	tuple origin_destination = _CLIENT_CLIENT_;

	/// <summary>
	/// ova funkcija odredjuje kome treba proslediti poruku
	/// ako je par client-client znaci da je poruka stigla na server sa strane izvorisnog klijenta i treba da se prosledi na bratski server
	/// funkcija modifikuje izvoriste kao server i salje bratskom serveru koji onda odredjuje klijenta kao destinaciju i prosledjuje mu poruku
	/// 
	/// </summary>
	/// <param name="message"></param>
	/// <returns></returns>
	if ((message->origin == _CLIENT_ && message->destination == _SERVER_) || (message->origin == _CLIENT_ && message->destination == _CLIENT_))
	{
		origin_destination = _CLIENT_SERVER_;
		message->origin = _SERVER_;
	}
	else if (message->origin == _SERVER_ && message->destination == _SERVER_)
	{
		origin_destination = _SERVER_SERVER_;
		message->destination = _CLIENT_;
	}
	else if (message->origin == _SERVER_ && message->destination == _CLIENT_)
	{
		origin_destination = _SERVER_CLIENT_;
	}

	return origin_destination;
}


/*
	Treci deo does_exist zahteva, ovaj deo obrade zahteva se izvrsava posto je zahtev stigao na drugi server
	odnosno ovaj deo zahteva proverava samo postojanje klijenta na serveru i vraca odgovor bratskom serveru
*/
void exists_reqest_part_three(MESSAGE* message, SOCKET brother_server,TYPE type)
{
	int index = map_type_to_index(type);


	while (WaitForSingleObject(client_hash_array[index].client_mutex, INFINITE) != WAIT_OBJECT_0);

	if (client_hash_array[index].socket == INVALID_SOCKET)
	{
		message->data.config.command = _NOT_EXISTS_;
	}
	else
	{
		message->data.config.command = _EXISTS_;
	}
	ReleaseMutex(client_hash_array[index].client_mutex);

	send_message_tcp(brother_server, *message);

}


/*
	Ova funkcija uzima klijentski soket i salje prosledjenu poruku ka klijentu

*/

void send_message_to_client(MESSAGE* message, TYPE type)
{
	int index = map_type_to_index(type);

	while (WaitForSingleObject(client_hash_array[index].client_mutex, INFINITE) != WAIT_OBJECT_0);

	MESSAGE_STATE state = FAULT;
	if (client_hash_array[index].socket != INVALID_SOCKET)
	{
		send_message_tcp(client_hash_array[index].socket,*message);
	}

	ReleaseMutex(client_hash_array[index].client_mutex);

}

void connect_error_handler()
{
	print_last_winsock_error();
}


/*
	Ovo je funkcija koja predstavlja obradjivacki thread na serveru

	Thread se budi na klijentski FullSemaphore odnosno kada se upise poruka na red thread biva signaliziran i budi se kako bi poruku obradio

	Thread se zavrsava signaliziranjem FinishSignal semaphore-a odnosno kada korisnik pritisne ENTER na serveru

*/
DWORD WINAPI request_worker(LPVOID lpParam)
{
	WORKER_PARAMS vars = *(WORKER_PARAMS*)lpParam;
	TYPE type = vars.queue_type;
	int client_index = map_type_to_index(type);


	Node** head = &queue_heads[map_type_to_index(type)];
	SOCKADDR_IN brother_address = vars.brother_address;
	HANDLE semaphores[2] = { FinishSignal,client_hash_array[client_index].FullSemaphore};

	SOCKET brother_socket = init_socket();
	
	while(!connect_to_address(brother_socket, brother_address));
	int wait_retval = 10;


	while ((wait_retval != WAIT_OBJECT_0))
	{
		while ((wait_retval = WaitForMultipleObjects(2, semaphores, FALSE, INFINITE)) == WAIT_OBJECT_0 + 1)
		{
			Node node = pop(head);

			MESSAGE message = node.data;
			tuple origin_destination = parse_output_location(&message);

			if (origin_destination == _CLIENT_SERVER_)
			{
				send_message_tcp(brother_socket, message);
			}
			else if (origin_destination == _SERVER_SERVER_)
			{
				exists_reqest_part_three(&message, brother_socket, type);
			}
			else if (origin_destination == _SERVER_CLIENT_)
			{
				int index = map_type_to_index(type);

				while (WaitForSingleObject(client_hash_array[index].client_mutex, INFINITE) != WAIT_OBJECT_0);

				MESSAGE_STATE state = FAULT;
				if (client_hash_array[index].socket != INVALID_SOCKET)
				{
					send_message_tcp(client_hash_array[index].socket, message);
				}
				else
				{
					push(head, get_new_node(&message));
				}

				ReleaseMutex(client_hash_array[index].client_mutex);
			}
			ReleaseSemaphore(client_hash_array[client_index].EmptySemaphore, 1, NULL);

			wait_retval = ~(WAIT_OBJECT_0 + 1);
			break;
		}
	}
	
	return 0;
}