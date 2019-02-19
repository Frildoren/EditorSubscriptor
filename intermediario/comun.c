/*
   Incluya en este fichero todas las implementaciones que pueden
   necesitar compartir todos los módulos (editor, subscriptor y
   proceso intermediario), si es que las hubiera.
*/

#include "comun.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

int connect_socket(char* address, int port)
{
	int sock;
	struct sockaddr_in server;
	struct hostent *host_info;
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if(sock < 0)
	{
		    perror("Could not create socket");
			return -1;
	}

	host_info=gethostbyname(address);
	memcpy(&server.sin_addr.s_addr, host_info->h_addr, host_info->h_length);

	//server.sin_addr.s_addr = inet_addr(address);
	server.sin_family = AF_INET;
	server.sin_port = htons( port );

	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		    perror("Could not connect to server");
		    return -1;
	}

	return sock;
}

int connect_inter()
{
	return connect_socket( getenv("SERVIDOR"), atoi(getenv("PUERTO")) );
}
