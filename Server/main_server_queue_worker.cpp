#include "main_server_queue_worker.h"



DWORD WINAPI PopMessageThread(LPVOID lpParam)
{

	while (true)
	{
		/*WaitForSingleObject(BufferSignal, INFINITE);
		Node povratna = pop_front(&cvor);
		printf_s("Procitao sam %c\n", povratna.data.queueName[0]);
		switch (povratna.data.queueName[0])
		{
		case 'i':
			printf("Konjska kola i maleni konji, osta kuca osta vakuf donji \n");break;
		case 'f':
			printf("Policija postavlja patrole,cuva zene od pijanog lole\n");break;
		case 'd':
			printf("Sve je nase odavde do pala samo nije rogatica mala \n");break;
		case 's':
			printf("Policijo stanice Sokoca gdje je pistolj pokojnog mi oca \n");break;
		case 'c':
			printf("Oj barabo bole li te rane gdje su lole cakijama bole \n");break;
		default:
			break;
		}*/
	}

	return 0;
}