#ifndef _COMMS_INTERFACE_H
#define _COMMS_INTERFACE_H

#include <WinSock2.h>


/// <summary>
/// Inicijalizuje socket i pokusava da poveze klijenta za zeljeni red, ako je povezivanje uspesno vraca socket kroz koji se moze komunicirati sa serverom, ako
/// je povezivanje neuspesno zatvara socket i vraca SOCKET_ERROR
/// </summary>
/// <param name="queueName"> Naziv reda na koji klijent zeli da se poveze, moguce vrednosti: char,int,float,double, short</param>
/// <returns>SOCKET / SOCKET_ERROR</returns>
SOCKET Connect(const char* queueName);

/// <summary>
/// Proverava da li na suprotnom serveru postoji povezan klijent, vraca true ili false u zavisnosti od postojanja klijenta
/// </summary>
/// <param name="queueName"></param>
/// <returns>true / false</returns>
bool Exist(const char* queueName);


/// <summary>
/// Ovaj deo interfejsa je vec implementiran kroz funkciju send_message_tcp u iz high_level_connection.h
/// </summary>
/// <param name="message"></param>
/// <param name="type"></param>
/// <returns></returns>
bool Send_Message(void* message, char type);

#endif