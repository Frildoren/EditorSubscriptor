#include "subscriptor.h"
#include "comun.h"
#include "edsu_comun.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>

struct notify_args
{
	void (*notif_evento)(const char *, const char *);
	void (*alta_tema)(const char *);
	void (*baja_tema)(const char *);
};


int sockfd, port;
pthread_t thread_id;

void* listen_notifications(void* args)
{
	int newsockfd;
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(struct sockaddr_in);

	while(1)
	{
		Message message;

		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

		if(read(newsockfd, &message, sizeof(Message)) < 0)
		{
			perror("Error reading from newsockfd");
			break;
		}

		switch(message.code)
		{
			case EVENT:
				((struct notify_args*)args)->notif_evento(message.topic, message.value);
				break;

			case CREATE:
				((struct notify_args*)args)->alta_tema(message.topic);
				break;

			case CANCEL:
				((struct notify_args*)args)->baja_tema(message.topic);
				break;
		}
	}

	return (void*) 0;
}

int alta_subscripcion_tema(const char *tema)
{
	int sock = connect_inter();

	Message message;
	message.code = SUBSCRIBE;
	strncpy(message.topic, tema, STRLEN);
	sprintf(message.value, "%d", port);

	write(sock, &message, sizeof(Message));
	bzero(&message, sizeof(Message));
	read(sock, &message, sizeof(Message));

	printf("Code received: %d", message.code);

	return message.code;
}

int baja_subscripcion_tema(const char *tema)
{
	int sock = connect_inter();
	Message message;
	message.code = UNSUBSCRIBE;
	strncpy(message.topic, tema, STRLEN);
	sprintf(message.value, "%d", port);


	write(sock, &message, sizeof(Message));
	bzero(&message, sizeof(Message));
	read(sock, &message, sizeof(Message));

	printf("Code received: %d", message.code);

	return message.code;
}

int inicio_subscriptor(void (*notif_evento)(const char *, const char *),
                void (*alta_tema)(const char *),
                void (*baja_tema)(const char *))
{

	struct sockaddr_in serv_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("ERROR opening socket");
		return 1;
	}

	int enable = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = 0;

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		return 1;
	}

	listen(sockfd, 20);

	socklen_t len = sizeof(serv_addr);
	if(getsockname(sockfd, (struct sockaddr *)&serv_addr, &len) < 0)
	{
		perror("Error retrieving socket info");
		return 1;
	}

	port = ntohs(serv_addr.sin_port);	

	struct notify_args* args = malloc(sizeof(struct notify_args));
	args->notif_evento = notif_evento;
	args->alta_tema = alta_tema;
	args->baja_tema = baja_tema;
	if(pthread_create(&thread_id, NULL, listen_notifications, (void*) args))
	{
		perror("Error starting new thread");
		return 1;
	}


	if(alta_tema != NULL)
	{
		//Sign up subscriber
		int sock = connect_inter();
		Message message;
		message.code = METASUBSCRIBE;
		sprintf(message.value, "%d", port);
		write(sock, &message, sizeof(Message));
		read(sock, &message, sizeof(Message));

		return message.code;
	}

	return 0;
}

int fin_subscriptor()
{
	pthread_cancel(thread_id);

	int sock = connect_inter();
	Message message;
	message.code = FIN;
	sprintf(message.value, "%d", port);
	write(sock, &message, sizeof(Message));
	read(sock, &message, sizeof(Message));

	printf("To return: %d\n", message.code);

	return message.code;
}

