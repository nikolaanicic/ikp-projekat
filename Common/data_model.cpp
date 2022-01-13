#define _CRT_SECURE_NO_WARNINGS
#include "data_model.h"



// Ova funkcija ispisuje sve karaktere koji se nalaze u baferu do odredjene duzine
// Koriscena je u debagovanju projekta
// Nema povratnu vrednost

void print_buffer(char* buffer, int buffer_len)
{
	if (buffer == NULL || buffer_len <= 0)
		return;

	for (int i = 0; i < buffer_len; i++)
	{
		printf("%c", buffer[i]);
	}
}

// Ova funkcija ispisuje data deo unije u strukturi poruke
// Koristi se pri ispisivanju poruke kada se pronadje da je deo podataka popunjen
// Nema povratnu vrednost

void print_data(DATA data, TYPE type)
{
	printf("\nDATA:");
	switch (type)
	{
	case _CHAR_: printf("%c", data.c); break;

	case _INT_:	printf("%d", data.i); break;
	case _SHORT_: printf("%d", data.s); break;
	case _FLOAT_: printf("%f", data.f); break;
	case _DOUBLE_: printf("%.2lf", data.d);  break;
	}
}

// Ova funkcija ispisuje komande odosno pravi mapiranje izmedju komandi na njihove const char*(string) vrednosti
// Nema povratnu vrednost
// Koristi se pri ispisu poruke kada je deo sa komandom popunjen

void print_command(COMMAND command)
{
	printf("\nCOMMAND:");
	switch (command)
	{
	case _CONNECT_: printf("CONNECT"); break;
	case _DOES_EXIST_: printf("DOES EXIST"); break;
	case _EXISTS_: printf("EXISTS"); break;
	case _MESSAGE_: printf("MESSAGE"); break;
	case _NOT_EXISTS_: printf("NOT EXISTS"); break;
	}
}

// Ova funkcija ispisuje lokaciju (izvoriste/odrediste) iz poruke, stvara mapiranje izmedju tipa LOCATION i string vrednosti vezane
// za konkretnu vrednost tipa LOCATION
// Nema povratnu vrednost

void print_location(LOCATION location)
{
	switch (location)
	{
	case _CLIENT_: printf("CLIENT"); break;
	case _SERVER_: printf("SERVER"); break;
	}
}

// Ova funkcija ispisuje tip podataka koji se moze naci u poruci
// Stvara mapiranje izmedju tipa TYPE i string vrednosti vezanih za taj tip
// Nema povratnu vrednost

void print_type(TYPE type)
{
	printf("\nTYPE:");
	switch (type)
	{
	case _INT_: printf("INT"); break;
	case _CHAR_: printf("CHAR"); break;
	case _SHORT_: printf("SHORT"); break;
	case _FLOAT_: printf("FLOAT"); break;
	case _DOUBLE_: printf("DOUBLE"); break;
	}
}


// Ova funkcija ispisuje deo sa podatkom u uniji strukture poruke kada je on popunjen
// Sluzi kao wrapper oko funkcija print_data
// Nema povratnu vrednost

void print_data_message(DATA_MESSAGE message)
{
	print_data(message.data, message.type);
}


// Wrapper oko funkcije print_command
// Nema povratnu vrednost

void print_config_message(CONFIG_MESSAGE message)
{
	print_command(message.command);
}


// Ova funkcija pronalazi koji deo poruke je popunjen i na osnovu toga poziva odgovarajucu funkciju za ispis
// INNER_MESSAGE je unija koja sadrzi ili podatak ili komandu
// FILLED_STRUCTURE je polje koje naznacava koji deo unije je popunjen
// Nema povratnu vrednost


void print_inner_message(INNER_MESSAGE message, FILLED_STRUCTURE has_data)
{
	switch (has_data)
	{
		case _CONFIG_: print_config_message(message.config); break;
		case _DATA_: print_data_message(message.data); break;
	}
}


// Ova funkcija ispisuje celu poruku odnosno sve njene uvek popunjene delove i svaki popunjen deo iz unija koja se mogu naci u poruci
// message je poruka koju treba ispisati
// Nema povratnu vrednost

void print_message(MESSAGE message)
{
	printf("\n*********");
	printf("\nORIGIN:"); print_location(message.origin);
	printf("\nDESTINATION:"); print_location(message.destination);
	printf("\nQUEUE NAME:%s", message.queueName);

	print_inner_message(message.data, message.has_data);
	printf("\n*********");

}

// Ova funkcija alocira char bafer zeljene velicine
// size je velicina bloka koju treba alocirati
// povratna vrednost je ili pokazivac na blok ako je alociranje uspesno ili NULL ako je neuspesno
// pri neuspesnom alociranju ispisuje se poruka radi debagovanja

char* allocate_buffer(int size)
{
	char* buffer = (char*)malloc(size);

	if (buffer == NULL)
	{
		printf("\nNo memory to allocate new message buffer");
	}

	return buffer;
}


// Ova funkcija oslobadja char bafer
void free_buffer(char** buffer)
{
	if (*buffer != NULL)
	{
		free(*buffer);
		*buffer = NULL;
	}
}


// Ova funkcija alocira blok memorije koji moze da drzi tacno jednu poruku
// Povratna vrednost je ili pokazivac na blok memorije za poruku ili NULL ako je alokacija neuspesna
// Ima ispis pri neuspesnoj alokaciji zbog debagovanja

MESSAGE* allocate_message()
{
	MESSAGE* message = (MESSAGE*)malloc(sizeof(MESSAGE));
	if (message == NULL)
	{
		printf("\nFailed to allocate message buffer");

	}

	return message;
}


// Ova funcija mapira TYPE na string predstavu konkretne vrednosti
// _CHAR_ -> "char" itd...

const char* map_type_to_queue_name(TYPE type)
{
	switch (type)
	{

	case _INT_: return "int";
	case _CHAR_: return "char";
	case _SHORT_: return "short";
	case _FLOAT_: return "float";
	case _DOUBLE_: return "double";
	}
}


/*
	Serijalizuje poruku u void* bafer koji se salje kroz mrezu
	
	Povratna vrednost ukazuje na tacnost prosledjenih parametara
	
*/
bool serialize_message(void* buffer, MESSAGE message)
{
	if (buffer == NULL)
		return false;

	memcpy(buffer, (char*)&message, sizeof(MESSAGE));

	return true;
}


/*
	Deserijalizuje poruku iz void* bafer u MESSAGE*
	Povratna vrednost ukazuje na tacnost prosledjenih parametara
	
	
*/
bool deserialize_message(void* buffer, MESSAGE* message)
{
	if (buffer == NULL || message == NULL)
		return false;

	memcpy(message, buffer, sizeof(MESSAGE));
	return true;
}



/*
	Ova funkcija popunjava deo unije podataka sa random generisanim podatkom
	
	Argumenti:
		type -> polje unije koje treba popuniti, odnosno u sta treba kastovati random generisani podatak
		data -> 'random' generisani bajtovi podataka koje treba kastovati
		d -> pokazivac na uniju cije polje treba popuniti

	Povratna vrednost ukazuje na validnost prosledjenih parametara

*/
bool populate_data(DATA* d, TYPE type, void* data)
{
	if (d == NULL || data == NULL)
		return false;

	switch (type)
	{
	case _CHAR_:
	{
		char c = *(char*)data;
		d->c = c;
		break;
	}

	case _INT_:
	{
		int i = *(int*)data;
		d->i = i;
		break;

	}
	case _SHORT_:
	{
		short s = *(short*)data;
		d->s = s;
		break;
	}
	case _FLOAT_:
	{
		float f = *(float*)data;
		d->f = f;
		break;
	}
	case _DOUBLE_:
	{
		double dou = *(double*)data;
		d->d = dou;
		break;
	}
	default:
		return false;
	}

	return true;
}


/*
	Ova funkcija popunjava config deo poruke proslednjenom komandom

	Povratna vrednost ukazuje na validnost pokazivaca

*/
bool populate_config_message(CONFIG_MESSAGE* message, COMMAND command)
{
	if (message == NULL)
		return false;

	message->command = command;


	return true;
}

/*
	Ova funkcija popunjava strukturu koja drzi uniju podataka i tip koji pokazuje koje polje je popunjeno unutar unije
	Poziva populate_data metodu sa prosledjenim parametrima

	Povratna vrednost pokazuje validnost podtaka i ispravnost popunjavanja unije

*/

bool populate_data_message(DATA_MESSAGE* message, TYPE type, void* data)
{
	if (message == NULL || data == NULL)
		return false;

	message->type = type;
	bool retval = populate_data(&(message->data), type, data);

	return retval;
}

/*
	Ova funkcija popunjava uniju viseg nivoa odnosno pakuje ili podatke ili komadnu

	Argumenti:
		message-> pokazivac na uniju koja se popunjava
		has_data -> polje unije koje treba popuniti
		data-> podatak ili komanda koje treba ubaciti u uniju

*/

bool populate_inner_message(INNER_MESSAGE* message, void* data, FILLED_STRUCTURE has_data)
{
	if (message == NULL || data == NULL)
		return false;

	switch (has_data)
	{
	case _CONFIG_:
	{
		memcpy(&(message->config), data, sizeof(CONFIG_MESSAGE));
		break;
	}
	case _DATA_:
	{
		memcpy(&(message->data), data, sizeof(DATA_MESSAGE));
		break;
	}
	}

	return true;
}


/*
	Mapira ime reda na TYPE 

	Aegument:
		queue_name -> ime reda: "char", "int", "float", "double", "short"

	Povratna vrednost:
		TYPE na koji se ime reda mapiralo: _CHAR_, _INT_, _FLOAT_, _DOUBLE_, _SHORT_
*/
TYPE map_queue_name_to_type(const char* queue_name)
{
	char c = queue_name[0];

	return c == 's' ? _SHORT_ : (TYPE)c;

}


/*
	Ova funkcija sluzi kao fasada koju korisnici biblioteke pozivaju kako bi napravili poruku u kojoj je popunjen deo unije koji drzi podtake

	Aegumenti:
		data -> pokazivac na podatak koji treba ubaciti u poruku
		type -> tip podatka koji treba popuniti

	Povratna vrednost:
		pokazivac na popunjenu poruku ili NULL

*/

MESSAGE* make_message_data(void* data, TYPE type)
{
	MESSAGE* message = allocate_message();
	if (message != NULL)
	{
		DATA_MESSAGE data_message;
		populate_data_message(&data_message, type, data);
		populate_inner_message(&(message->data), (void*)&data_message, _DATA_);

		message->destination = _CLIENT_;
		message->has_data = _DATA_;
		message->origin = _CLIENT_;


		strcpy_s(message->queueName, map_type_to_queue_name(type));
	}
	return message;
}


/*
	Ova funkcija sluzi kao fasada koju korisnici biblioteke pozivaju kako bi popunili deo unije koji sadrzi komandu

	Argumenti:
		queueName -> tip reda za koji je poruka namenjena
		command -> komanda koja se salje
		origin -> izvoriste poruke
		destination -> odrediste poruke

	Povratna vrednost:
		pokazivac na popunjenu poruku ili NULL

*/
MESSAGE* make_message_config(TYPE queueName, COMMAND command, LOCATION origin, LOCATION destination)
{

	MESSAGE* message = allocate_message();
	if (message != NULL)
	{

		CONFIG_MESSAGE config_message;
		populate_config_message(&config_message, command);
		populate_inner_message(&(message->data), (void*)(&config_message), _CONFIG_);

		message->has_data = _CONFIG_;
		message->destination = destination;
		message->origin = origin;

		strcpy(message->queueName, map_type_to_queue_name(queueName));

	}
	return message;
}


/*
	Ova funkcija oslobadja blok memorije koji je bio zauzet za poruku/ niz poruka

	Nema povratnu vrednost
*/
void free_message(MESSAGE** message)
{
	if (*message != NULL)
	{
		free(*message);
		*message = NULL;
	}
}
