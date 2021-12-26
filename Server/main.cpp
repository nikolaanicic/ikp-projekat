#include "server_config.h"
#include "server_main_thread.h"
#include <conio.h>
#include "queue.h"
#include "synchronization.h"



int main()
{
	
	init_winsock(MAKEWORD(2, 2));

	SOCKADDR_IN server_address;

	unsigned short server_port = get_server_port();
	
	populateAddressV4(&server_address, "127.0.0.1", server_port);
	if (!init_server(server_address))
	{
		stop_server();
		return 12;
	}

	DWORD load_balancer_id;
	HANDLE load_balancer_th;


	load_balancer_th = CreateThread(NULL, 0, load_balancer, NULL, 0, &load_balancer_id);



	printf("\nPritisnite [Enter] ds ugasite server");
	_getch();

	stop_server();
	WaitForSingleObject(load_balancer_th, INFINITE);

	safe_close_handle(load_balancer_th);

	return 0;
}