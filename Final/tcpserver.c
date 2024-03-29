/* tcpserver.c */
/* Programmed by Adarsh Sethi */
/* September 13, 2017 */    

#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, bind, listen, accept */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define STRING_SIZE 1024   

/* SERV_TCP_PORT is the port number on which the server listens for
   incoming requests from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_TCP_PORT 48500
#define Secret_Code 2447
#define Name "Aditya-Karmarkar"


/**
 * this is structure for message
 */
struct Messages {
   unsigned short client_secret_code;
   unsigned short server_secret_code;
   unsigned short name_size;
   char  name[80];
};
static const struct Messages EmptyMessage;

int getmessageSize(struct Messages message){
	int length = 0;
	
	length =  sizeof(message.client_secret_code) +  sizeof(message.server_secret_code) + sizeof(message.name_size) + (int)ntohs(message.name_size);
	//printf("TCP :%d \n", length);
	return length;
}
int main(void) {

   int sock_server;  /* Socket on which server listens to clients */
   int sock_connection;  /* Socket on which server exchanges data with client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   unsigned int server_addr_len;  /* Length of server address structure */
   unsigned short server_port;  /* Port number used by server (local port) */

   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
   unsigned int client_addr_len;  /* Length of client address structure */

   int bytes_sent, bytes_recd; /* number of bytes sent or received */
   unsigned int i;  /* temporary loop variable */
   unsigned short client_secret_code;
   unsigned short server_secret_code = Secret_Code;
   struct Messages message; // this is structure that holds message sent by server
   struct Messages receivedMessage; // this is a structure that holds message received from client
   
   /* open a socket */

   if ((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      //perror("Server: can't open stream socket");
      exit(1);                                                
   }

   /* initialize server address information */
    
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */ 
   server_port = SERV_TCP_PORT; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */

   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      //perror("Server: can't bind to local address");
      close(sock_server);
      exit(1);
   }                     

   /* listen for incoming requests from clients */

   if (listen(sock_server, 50) < 0) {    /* 50 is the max number of pending */
      //perror("Server: error on listen"); /* requests that will be queued */
      close(sock_server);
      exit(1);
   }
   //printf("I am here to listen ... on port %hu\n\n", server_port);
  
   client_addr_len = sizeof (client_addr);

   /* wait for incoming connection requests in an indefinite loop */

   for (;;) {

      sock_connection = accept(sock_server, (struct sockaddr *) &client_addr, 
                                         &client_addr_len);
                     /* The accept function blocks the server until a
                        connection request comes from a client */
      if (sock_connection < 0) {
         //perror("Server: accept() error\n"); 
         close(sock_server);
         exit(1);
      }
 
      /* receive the message */
	  		receivedMessage = EmptyMessage;

		bytes_recd = recv(sock_connection, &receivedMessage, sizeof(struct Messages), 0); //expect all 86 bytes
		//printf("client_secret %d\n",htons(receivedMessage.client_secret_code));
		//printf("server_secret %d\n",htons(receivedMessage.server_secret_code));
		//printf("size %d\n",htons(receivedMessage.name_size));
		//printf("Name %s\n",receivedMessage.name);
      //bytes_recd = recvfrom(sock_connection, &receivedMessage, sizeof(struct Messages), 0,(struct sockaddr *) &client_addr, &client_addr_len);
	  if (bytes_recd > 0){
        /* prepare the message to send */
		 message.server_secret_code = htons(server_secret_code);
		 //printf("%d\n", htons(server_secret_code));
         client_secret_code = ntohs(receivedMessage.client_secret_code);
		 //printf("Client Secret code %d",client_secret_code);
		 /* send message */
		 message.client_secret_code = htons(client_secret_code);
		 message.name_size = htons(0);
		 message.name[0]='\0'; //send empty string
		 //send message
		bytes_sent = send(sock_connection, &message, getmessageSize(message), 0); //send limited bytes
		//bytes_sent = sendto(sock_connection, &message, sizeof(struct Messages), 0,(struct sockaddr *) &client_addr, sizeof (client_addr));

      }
		message = EmptyMessage;
		receivedMessage = EmptyMessage;
      /* close the socket */
      close(sock_connection);

	  }
   } 

