#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "high_level_connection.h"

#pragma comment(lib, "Ws2_32.lib")


/*
	Ova funkcija serijalizuje prosledjenu poruku i salje je kroz mrezu preko prosledjenog socketa

	Argumenti:
		socket  -> socket kroz koji treba poslati poruku, funkcija pretpostavlja da je socket ispravan, odnosno postoji, otvoren i podesen za slanje
		message -> poruka koju treba poslati preko mreze

	Povratna vrednost:
		prikazuje uspesnost serijalizacije i slanja poruke, moguce vrednosti: FAULT/SUCCESS

*/

MESSAGE_STATE send_message_tcp(SOCKET socket, MESSAGE message)
{
	MESSAGE_STATE retval = FAULT;

	char* send_buffer = allocate_buffer(sizeof(MESSAGE));
	if (!serialize_message(send_buffer, message))
	{
		free(send_buffer);
		return FAULT;
	}

	int iRes = send(socket, send_buffer, sizeof(MESSAGE), 0);


	if (iRes > 0)
	{
		retval = SUCCESS;
	}
	else if (iRes == SOCKET_ERROR)
	{
		retval = FAULT;
	}

	free(send_buffer);
	

	return retval;
}


/*
	Ova funkcija pokusava da primi poruku preko prosledjenog socketa i deserijalizuje u prosledjen joj bafer
	Ako je uspesno primljena poruka deserijalizuje je u bafer, ako dodje do greske ili shutdown signala zatvara socket

	Argumenti:
		s -> socket kroz koji treba primiti poruku
		message -> pokazivac na prethodno zauzeti bafer u koji treba deserijalizovati poruku

	Povratna vrednost:
		pokazuje uspesnost prijema i deserijalizacije poruke
		moguce vrednosti: FAULT, SUCCESS i DISCONNECT ako je primljen shutdown signal na socketu

*/
MESSAGE_STATE receive_message_tcp(SOCKET s, MESSAGE* message)
{
	MESSAGE_STATE retval = FAULT;

	if (message == NULL || s == INVALID_SOCKET)
		return retval;

	char* recv_buffer = allocate_buffer(sizeof(MESSAGE));

	int iRes = recv(s, recv_buffer, sizeof(MESSAGE), 0);

	if (iRes > 0)
	{
		if (deserialize_message(recv_buffer, message))
		{
			retval = SUCCESS;
		}
	}
	else if (iRes == SOCKET_ERROR)
	{
		close_socket(s);
		retval = FAULT;
	}
	else if (iRes == 0)
	{
		printf("\nConnection closed");
		close_socket(s);
		retval = DISCONNECT;
	}

	free(recv_buffer);

	return retval;
}
