#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "high_level_connection.h"

#pragma comment(lib, "Ws2_32.lib")


MESSAGE_STATE send_message_tcp(SOCKET socket, MESSAGE message)
{
	MESSAGE_STATE retval = FAULT;

	char* send_buffer = allocate_buffer(sizeof(MESSAGE));
	if (!serialize_message(send_buffer, message))
	{
		free(send_buffer);
		printf("\nFailed to serialize message");
		return FAULT;
	}

	int iRes = send(socket, send_buffer, sizeof(MESSAGE), 0);


	if (iRes > 0)
	{
		printf("\nMESSAGE SENT SUCCESSFULLY");
		retval = SUCCESS;
	}
	else if (iRes == SOCKET_ERROR)
	{
		printf("\nMESSAGE NOT SENT");
		retval = FAULT;
	}

	free(send_buffer);

	return retval;
}

MESSAGE_STATE receive_message_tcp(SOCKET s, MESSAGE* message)
{
	MESSAGE_STATE retval = FAULT;

	if (message == NULL || s == INVALID_SOCKET)
		return retval;

	char* recv_buffer = allocate_buffer(sizeof(MESSAGE));

	int iRes = recv(s, recv_buffer, sizeof(MESSAGE), 0);

	if (iRes > 0)
	{
		printf("\nMESSAGE SUCCESSFULLY RECEIVED");
		if (deserialize_message(recv_buffer, message))
		{
			printf("\nMESSAGE SUCCESSFULLY DESERIALIZED");
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
