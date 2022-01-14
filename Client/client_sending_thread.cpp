#include "client_sending_thread.h"


int get_menu_option()
{

	int option = -1;

	do
	{
		EnterCriticalSection(&console_section);
		printf("\n1.Send random message");
		printf("\n2.Send does exist message");
		printf("\n3.Ugasi klijenta");
		LeaveCriticalSection(&console_section);
		scanf_s("%d", &option);

		if (option != 1 && option != 2 && option != 3)
		{
			option = -1;
		}

	} while (option == -1);

	return option;
}



int stress_test_mode(TYPE type, DWORD period)
{
	MESSAGE* message = NULL;
	MESSAGE_STATE state;
	while (WaitForSingleObject(FinishSignal, period) != WAIT_OBJECT_0)
	{
		void* random_data = get_random_data(type);
		message = make_message_data(random_data, type);

		while (WaitForSingleObject(socket_mutex, INFINITE) != WAIT_OBJECT_0);

		state = send_message_tcp(client, *message);

		if (state == FAULT)
		{
			ReleaseMutex(socket_mutex);
			ReleaseSemaphore(FinishSignal, 1, NULL);
			free_void_buffer(&random_data);
			free_message(&message);

			return 0;
		}

		ReleaseMutex(socket_mutex);
		free_void_buffer(&random_data);
		free_message(&message);
		message = NULL;
	}


	return 0;
}


MESSAGE* get_optioned_message(int option, TYPE type)
{
	MESSAGE* message = NULL;
	void* random_data = NULL;
	switch (option)
	{
	case 1:
		random_data = get_random_data(type);
		message = make_message_data(random_data, type);
		break;
	case 2:
		message = make_message_config(type, _DOES_EXIST_, _CLIENT_, _SERVER_);
		break;

	case 3:
		ReleaseSemaphore(FinishSignal, 3, NULL);
		free_void_buffer(&random_data);
		return NULL;
	}


	free_void_buffer(&random_data);
	return message;
}

int menu_mode(TYPE type)
{
	MESSAGE* message = NULL;
	MESSAGE_STATE state;
	while (WaitForSingleObject(FinishSignal, 500) != WAIT_OBJECT_0)
	{
		message = get_optioned_message(get_menu_option(), type);

		while (WaitForSingleObject(socket_mutex, INFINITE) != WAIT_OBJECT_0);
		if (message != NULL)
			state = send_message_tcp(client, *message);
		else
			state = FAULT;

		if (state == FAULT)
		{
			ReleaseMutex(socket_mutex);
			ReleaseSemaphore(FinishSignal, 1, NULL);
			free_message(&message);

			return 0;
		}
		ReleaseMutex(socket_mutex);
		free_message(&message);
		message = NULL;
	}

	return 0;
}


DWORD get_waiting_period(bool stress_test = false)
{
	DWORD retval = -1;

	if (!stress_test)
	{
		do
		{
			EnterCriticalSection(&console_section);
			printf("\nUnesite period [ >= 0 ] ms na koji ce klijent slati poruke:");
			scanf_s("%d", &retval);
			LeaveCriticalSection(&console_section);


		} while (retval <= 0);
	}
	else
	{
		retval = STRESS_TEST_PERIOD;
	}

	return retval;
}



DWORD WINAPI RunSendingThread(LPVOID lpParam)
{
	TYPE type = *(TYPE*)lpParam;

	MESSAGE_STATE state = FAULT;

	printf("\nStarted client sending thread");


	if (mode == _STRESS_TEST_)
	{
		DWORD period = get_waiting_period(true);
		return stress_test_mode(type, period);
	}
	else if (mode == _MENU_)
	{
		return menu_mode(type);
	}

	return 0;
}