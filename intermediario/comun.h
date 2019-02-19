/*
   Incluya en este fichero todas las definiciones que pueden
   necesitar compartir todos los módulos (editor, subscriptor y
   proceso intermediario), si es que las hubiera.
*/
#define STRLEN		256

#define ERROR			-1
#define OK				0
#define METASUBSCRIBE	1
#define SUBSCRIBE		2
#define UNSUBSCRIBE		3
#define FIN				4
#define EVENT			7
#define CREATE			8
#define CANCEL			9

typedef struct Message
{
	int code;
	char topic[STRLEN];
	char value[STRLEN];
} Message;

int connect_inter();
int connect_socket(char* address, int port);
