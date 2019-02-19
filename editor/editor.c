#include "editor.h"
#include "comun.h"
#include "edsu_comun.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int generar_evento(const char *tema, const char *valor) {
	int sock = connect_inter();

	Message message;
	message.code = EVENT;
	strncpy(message.topic, tema, STRLEN);
	strncpy(message.value, valor, STRLEN);

	write(sock, &message, sizeof(Message));
	read(sock, &message, sizeof(Message));

	printf("Code received: %i", message.code);

	return message.code;
}

/* solo para la version avanzada */
int crear_tema(const char *tema) {

	int sock = connect_inter();

	Message message;
	message.code = CREATE;
	strncpy(message.topic, tema, STRLEN);

	write(sock, &message, sizeof(Message));
	read(sock, &message, sizeof(Message));

	printf("Code received: %i", message.code);

	return message.code;
}

/* solo para la version avanzada */
int eliminar_tema(const char *tema) {
	int sock = connect_inter();

	Message message;
	message.code = CANCEL;
	strncpy(message.topic, tema, STRLEN);

	write(sock, &message, sizeof(Message));
	read(sock, &message, sizeof(Message));

	printf("Code received: %i", message.code);

	return message.code;
}

