#include "server_config.h"
#include "server_main_thread.h"
#include <conio.h>
#include "queue.h"
#include "synchronization.h"
#include "main_server_queue_worker.h"



int main()
{
	init_winsock(MAKEWORD(2, 2));

	HANDLE workers[WORKER_NUM];
	DWORD worker_ids[WORKER_NUM];

	DWORD load_balancer_id;
	DWORD main_worker_id;

	HANDLE load_balancer_th;
	HANDLE main_worker;


	SOCKADDR_IN brother = server_starter();
	WORKER_PARAMS* params = (WORKER_PARAMS*)malloc(sizeof(WORKER_PARAMS) * WORKER_NUM);
	if (params == NULL)
	{
		safe_close_server();
		exit(11);
	}

	TYPE types[WORKER_NUM] = { _CHAR_,_INT_,_FLOAT_,_SHORT_,_DOUBLE_ };
	for (int i = 0; i < WORKER_NUM; i++)
	{
		int index = map_type_to_index(types[i]);
		make_worker_params(&params[i], &client_hash_array[index].socket, client_hash_array[index].client_mutex, types[i],brother);
		workers[i] = CreateThread(NULL, 0, request_worker, &(params[i]), 0, &worker_ids[i]);
		if (!workers[i])
		{
			printf("\nFailed to start one of the worker threads");
			safe_close_server();
			for (int j = 0; j < i; j++)
			{
				safe_close_handle(workers[j]);
				exit(5);
			}
		}

	}

	load_balancer_th = CreateThread(NULL, 0, load_balancer, NULL, 0, &load_balancer_id);
	main_worker = CreateThread(NULL, 0, PopMessageThread, NULL, 0, &main_worker_id);


	if (!load_balancer_th || !main_worker)
	{
		printf("\nFailed to start server threads correctly");
		safe_close_server();
		safe_close_handle(load_balancer_th);
		safe_close_handle(main_worker);
		
		for (int i = 0; i < WORKER_NUM; i++)
		{
			safe_close_handle(workers[i]);
		}
		exit(12);
	}

	printf("\nPritisnite [Enter] da ugasite server");
	_getch();

	safe_close_server();

	WaitForSingleObject(load_balancer_th, INFINITE);
	WaitForSingleObject(main_worker, INFINITE);
	
	for (int i = 0; i < WORKER_NUM; i++)
	{
		WaitForSingleObject(workers[i], INFINITE);
	}

	safe_close_handle(load_balancer_th);
	safe_close_handle(main_worker);
	
	for (int i = 0; i < WORKER_NUM; i++)
	{
		safe_close_handle(workers[i]);
	}

	return 0;
}