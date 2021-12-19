#ifndef _SERVER_DATA_MODEL_H
#define _SERVER_DATA_MODEL_H

#include "data_model.h"
#include <WinSock2.h>


typedef struct _hash_array_node_
{
	SOCKET socket;
	SOCKADDR_IN client_address;
}HASH_NODE;

#define HASH_ARRAY_LEN (7)


int map_type_to_index(TYPE type);
void init_hash_array();


//tipovi su mapirani na osnovu ostatka deljenja sa 7
// moguci tipovi su char,int,short,float,double

// TIP		|  ASCII  |  DEC
//-----------------------------
// CHAR		|    c    |   99
// DOUBLE   |    d    |   100
// FLOAT	|    f    |   102
// SHORT    |    h    |   104
// INT		|    i    |   105



// polja na koja bi se mapirali e, f, g ostaju prazna

// TIP		|  ARRAY INDEX  |
// --------------------------
// INT		|		0		|
// CHAR		|		1		|
// DOUBLE	|		2		|
// FLOAT	|		4		|
// SHORT	|		6		|





#endif
