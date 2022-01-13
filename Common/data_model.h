#ifndef _DATA_MODEL_H
#define _DATA_MODEL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE (512)


/// <summary>
/// TYPE predstavlja skup tipova redova na koje klijent moze biti povezan i nad kojim moze zahtevati operacije
/// vrednosti su ovako izabrane jer predstavljaju ascii vrednosti koje po modulu 7 daju nepreklapajuci skup vrednosti koje mogu
/// biti iskorscene za pravljenje hes niza na serveru
/// </summary>
enum TYPE { _CHAR_ = 99, _DOUBLE_ = 100, _FLOAT_ = 102, _SHORT_ = 104, _INT_ = 105 };

/// <summary>
/// tip komandi i odgovora koje klijenti mogu poslati/ocekivati od servera
/// komande su sakrivene od korisnika koji sa serverom komunicira kroz zadati interfejs iz fajla comms_interface.h
/// </summary>
enum COMMAND { _CONNECT_ = 1, _DOES_EXIST_ = 2, _MESSAGE_ = 3, _EXISTS_ = 4, _NOT_EXISTS_ = 5 };

/// <summary>
/// izvorise/odrediste poruke, koristi se za rutiranje odnosno dedukciju odredista poruke
/// </summary>
enum LOCATION { _SERVER_ = 1, _CLIENT_ = 2 };

/// <summary>
/// stanje primljene/poslate poruke
/// </summary>
enum MESSAGE_STATE { SUCCESS = 0, FAULT = 1,DISCONNECT = 2 };

/// <summary>
/// odredjuje popunjenost unije, odnosno da li je popunjen deo sa porukom ili deo sa komandom
/// </summary>
enum FILLED_STRUCTURE {_DATA_ = 0, _CONFIG_ = 1};


/// <summary>
/// DATA unija predstavlja podatak koji klijent salje ka serveru, koristi se unija radi lakseg 
/// koda na serveru odnosno dovoljan je samo jedan red koji se moze korisiti za rad sa svim tipovima klijenta
/// </summary>

typedef union __data__
{
	char c;
	int i;
	double d;
	float f;
	short s;
}DATA;

/// <summary>
/// posto se koristi unija kao podatak a potrebno je obraditi vise tipova klijenata, potrebno je znati i koji je tip podatka primljen
/// pa je unija wrapovana strukturom koja sadrzi tip podatka
/// </summary>

typedef struct _data_message_
{
	DATA data;
	TYPE type;
}DATA_MESSAGE;


/// <summary>
/// struktura koja predstavlja komandu
/// </summary>

typedef struct _config_message_
{
	COMMAND command;

}CONFIG_MESSAGE;



/// <summary>
/// unija koja sadrzi podatak ili komandu
/// </summary>

typedef union _inner_message_
{
	CONFIG_MESSAGE config;
	DATA_MESSAGE data;

}INNER_MESSAGE;


/// <summary>
/// struktura poruke koja se salje kroz mrezu, polja queueName je ostavljeno jer je u zadatku trazeno da klijent salje queueName, ali je moglo biti zamenjeno
/// polje tipa TYPE
/// </summary>
typedef struct _message_
{
	LOCATION destination;
	LOCATION origin;
	FILLED_STRUCTURE has_data;
	char queueName[7];
	INNER_MESSAGE data;
}MESSAGE;


/// <summary>
/// ispisuje sve popunjene delove poruke
/// </summary>
/// <param name=""></param>
void print_message(MESSAGE);

/// <summary>
/// alocira char buffer odredjene velicine
/// </summary>
/// <param name="size"></param>
/// <returns></returns>
char* allocate_buffer(int size);
void free_buffer(char**);


/// <summary>
/// pravi poruku sa zeljenom komandom, odnosno popunjava config unije u strukturi poruke
/// </summary>
/// <param name="queueType"></param>
/// <param name="command"></param>
/// <param name="origin"></param>
/// <param name="destination"></param>
/// <returns></returns>
MESSAGE* make_message_config(TYPE queueType, COMMAND command, LOCATION origin, LOCATION destination);

/// <summary>
/// pravi poruku podataka, odnosno popunjava data deo unije u strukturi poruke
/// </summary>
/// <param name="data"></param>
/// <param name="type"></param>
/// <returns></returns>
MESSAGE* make_message_data(void* data, TYPE type);


/// <summary>
/// mapira queueName na tip TYPE, "char" - _CHAR_, "short" - _SHORT_ itd...
/// </summary>
/// <param name="queue_name"></param>
/// <returns></returns>
TYPE map_queue_name_to_type(const char* queue_name);

/// <summary>
/// obrnuto od funkcije iznad
/// </summary>
/// <param name="type"></param>
/// <returns></returns>
const char* map_type_to_queue_name(TYPE type);


/// <summary>
/// oslobadja memoriju alociranu za poruku
/// </summary>
/// <param name=""></param>
void free_message(MESSAGE**);

/// <summary>
/// alocira memoriju za jednu poruku
/// </summary>
/// <returns></returns>
MESSAGE* allocate_message();

/// <summary>
/// serijalizuje poruku iz message u void bafer koji se salje preko mreze
/// </summary>
/// <param name="buffer"></param>
/// <param name="message"></param>
/// <returns></returns>
bool serialize_message(void* buffer, MESSAGE message);


/// <summary>
/// deserijalizuje poruku koja je primljena preko mreze i iz void* je kopira u MESSAGE* bafer sa kojim server ili klijent rade
/// </summary>
/// <param name="buffer"></param>
/// <param name="message"></param>
/// <returns></returns>
bool deserialize_message(void* buffer, MESSAGE* message);

#endif
