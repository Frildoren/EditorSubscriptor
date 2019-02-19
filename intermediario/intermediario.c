#define _GNU_SOURCE

#include "comun.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct SubscriberNode
{
	char* address;
	int port;
	struct SubscriberNode* next;
} SubscriberNode;

typedef struct SubscriberList
{
	SubscriberNode head;
	int count;
} SubscriberList;

typedef struct TopicNode
{
    char* name;
    SubscriberList subscribers;
    struct TopicNode* next;
} TopicNode;
 
typedef struct TopicList
{
    TopicNode head;
    int count;
} TopicList;



TopicList topics;

int for_each_subscriber( SubscriberList* list, int (*f)(SubscriberNode*, int) )
{
	SubscriberNode* subscriber = &list->head;
	int counter = 0;
	while (subscriber->next != NULL)
	{
		subscriber = subscriber->next;
		if((*f)(subscriber, counter++) != 0)
		{
			counter--;
			break;
		}
	}
	return ++counter;
}

int add_subscriber(SubscriberList* list, char* address, int port)
{
	int find_subscriber(SubscriberNode* sn, int pos)
	{
		if(!strcmp(sn->address, address) && sn->port == port)
		{
			return 1;
		}
		return 0;
	};

	if(for_each_subscriber(list, find_subscriber) > list->count)
	{
		SubscriberNode* node = malloc(sizeof(SubscriberNode));
		node->address = strdup(address);
		node->port = port;

		node->next = list->head.next;
		list->head.next = node;
		list->count++;

		return 0;
	}
	return 1;
}

char* get_peer_address(int newsockfd)
{
	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(struct sockaddr_in);
	getpeername(newsockfd, (struct sockaddr*) &addr, &addr_size);
	char* remote = malloc(sizeof(char)*20);
	strcpy(remote, inet_ntoa(addr.sin_addr));

	return remote;
}

int for_each_topic( TopicList* list, int (*f)(TopicNode*, int) )
{
	TopicNode* topic = &list->head;
	int counter = 0;
    while (topic->next != NULL)
    {
        topic = topic->next;
        if((*f)(topic, counter++) != 0)
		{
			counter--;
			break;
		}
    }

	return ++counter;
}

int remove_subscriber(SubscriberList* sl, char* address, int port)
{
	int ret = 0;
	SubscriberNode* last = &sl->head;
	int remove_subscriber_(SubscriberNode* sn, int pos)
	{
		if(!strcmp(sn->address, address) && sn->port == port)
		{
			last->next = sn->next;
			sl->count--;
			free(sn);
			return ret = 1;
		}
		last = sn;
		return 0;
	}
	for_each_subscriber(sl, remove_subscriber_);
	return ret;
}

void notify_subscribers(SubscriberList* sl, Message message)
{
	int notify_subscriber(SubscriberNode* sn, int pos)
	{
		int sock = connect_socket(sn->address, sn->port);
		write(sock, &message, sizeof(Message));
		return 0;
	}
	for_each_subscriber(sl, notify_subscriber);
}

int find_topic(char* name, void (*callback)(TopicNode*, int))
{
	int find_topic_(TopicNode* tn, int pos)
	{
		if(!strcmp(name, tn->name))
		{
			if(callback != NULL)
				callback(tn, pos);
			return 1;
		}
		return 0;
	}
	return for_each_topic(&topics, find_topic_) > topics.count ? 0 : 1;
}

int create_topic(char* topic)
{

	if( ! find_topic(topic, NULL) )
	{
		// Generate new topic node
		TopicNode* node = malloc(sizeof(TopicNode));
		node->name = strdup(topic);
		
		// Insert new node as first
		node->next = topics.head.next;
		topics.head.next = node;
		topics.count++;

		return 1;
	}

	return 0;
}


int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Uso: %s puerto fichero_temas\n", argv[0]);
		return 1;
	}

	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t sread;

	// Read topics file
	fp = fopen(argv[2], "r");
	if (fp == NULL)
	{
		fprintf(stderr, "%s: Can not open file %s\n", argv[0], argv[2]);
		return 1;
	}


	// Create all topics from file
	while ((sread = getline(&line, &len, fp)) != -1)
	{
		strtok(line, "\n");
		create_topic(line);
	}
	
	// Initialize structures
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;

	// call to socket() 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	int enable = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	// Initialize socket structure
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	// Bind the host address
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}

	listen(sockfd, 20);
	clilen = sizeof(cli_addr);

	// Default error reply
	Message errorMessage;
	errorMessage.code = ERROR;

	// Now start listening for the clients
	printf("Starting listening...\n\n");
	while(1)
	{	
		// Accept actual connection from the client
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

		if (newsockfd < 0) {
			perror("ERROR on accept");
			exit(1);
		}
	
		Message message;
		Message ret_message = errorMessage;
		if( read(newsockfd, &message, sizeof(Message)) < 0 )
		{
			perror("Error reading from socket");
		}
		else
		{
			switch(message.code)
			{
				case METASUBSCRIBE: ;
					char* address = get_peer_address(newsockfd);
					int port = atoi(message.value);
					int ret = add_subscriber(&topics.head.subscribers, address, port);
					ret_message.code = ret ? OK : ERROR;

					int notify_existing(TopicNode* tn, int pos)
					{
						Message event;
						event.code = CREATE;
						strcpy(event.topic, tn->name);
						int sock = connect_socket(address, port);
						write(sock, &event, sizeof(Message));
						return 0;
					};
					for_each_topic(&topics, notify_existing);

					break;

				case SUBSCRIBE: ;
					address = get_peer_address(newsockfd);
					port = atoi(message.value);
					void subscribe(TopicNode* tn, int pos)
					{
						int res = add_subscriber(&(tn->subscribers), address, port);
						ret_message.code = res ? ERROR : OK;
					};
					find_topic(message.topic, subscribe);

					break;

				case UNSUBSCRIBE: ;
					address = get_peer_address(newsockfd);
					port = atoi(message.value);
					void unsubscribe(TopicNode* tn, int pos)
					{
						int ret = remove_subscriber(&tn->subscribers, address, port);
						ret_message.code = ret ? OK : ERROR;
					};
					find_topic(message.topic, unsubscribe);

					break;

				case EVENT: ;
					void event(TopicNode* tn, int pos)
					{
						notify_subscribers(&tn->subscribers, message);
						ret_message.code = OK;
					}
					
					find_topic(message.topic, event);

					break;

				case CREATE: ;
					if( create_topic(message.topic) )
					{
						notify_subscribers(&topics.head.subscribers, message);
						ret_message.code = OK;
					}

					break;

				case CANCEL: ; 
					TopicNode* last = &topics.head;
					int find_topic_cancel(TopicNode* tn, int pos)
					{
						if(!strcmp(message.topic, tn->name))
						{
							notify_subscribers(&topics.head.subscribers, message);
							last->next = tn->next;
							topics.count--;
							free(tn);
							ret_message.code = OK;
							return 1;
						}
						last = tn;
						return 0;
					};
					for_each_topic(&topics, find_topic_cancel);

					break;

				case FIN: ;
					address = get_peer_address(newsockfd);
					port = atoi(message.value);
					int remove_subscriber_(TopicNode* tn, int pos)
					{
						remove_subscriber(&tn->subscribers, address, port);
						return 0;
					};
					for_each_topic(&topics, remove_subscriber_);
					ret = remove_subscriber(&topics.head.subscribers, address, port);
					ret_message.code = ret ? OK : ERROR;

					break;

				default:
					printf("Unknown code %d\n", message.code);
			}
		}

		write(newsockfd, &ret_message, sizeof(Message));
	}


	return 0;
}
