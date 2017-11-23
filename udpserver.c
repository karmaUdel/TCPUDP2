/* udp_server.c */
/* Programmed by Adarsh Sethi */
/* September 13, 2017 */

#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, sendto, and recvfrom */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include <time.h>

#define STRING_SIZE 1024

/* SERV_UDP_PORT is the port number on which the server listens for
   incoming messages from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_UDP_PORT 48500
#define Secret_Code 2447
#define Name "Aditya-Karmarkar"


struct Messages {
   unsigned short client_secret_code;
   unsigned short server_secret_code;
   char  name[80];
};

void writeToFile(struct Messages message,struct Messages receivedMessage){
  FILE * fp;
   int i;
   /* open the file for writing*/
   fp = fopen ("c:\\temp\\serverInfo.txt","a");
 
   /* write 10 lines of text into the file stream*/
       fprintf (fp, "------------------\n");
	   fprintf (fp, "Port Number  %d\n",i); // this is port number
   	   fprintf (fp, "TCP Message sent:\t   %d\n",message.client_secret_code); // this is port number
	   fprintf (fp, "TCP Message received:  %d, %d\n",receivedMessage.client_secret_code,receivedMessage.server_secret_code);
   	   fprintf (fp, "UDP Message sent:\t   %d, \%d, %s\n",message.client_secret_code,message.server_secret_code,message.name); // this is port number
	   fprintf (fp, "UDP Message received:  %d, %d, %s\n",receivedMessage.client_secret_code,receivedMessage.server_secret_code,receivedMessage.name);
	   fprintf (fp, "------------------\n");
   
   /* close the file*/  
   fclose (fp);

}

int main(void) {
	srand(time(0)); // set seed for random number
   int sock_server;  /* Socket on which server listens to clients */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   unsigned short server_port;  /* Port number used by server (local port) */

   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
   unsigned int client_addr_len;  /* Length of client address structure */

   char sentence[STRING_SIZE];  /* receive message */
   char modifiedSentence[STRING_SIZE]; /* send message */
   unsigned int msg_len;  /* length of message */
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
   unsigned int i;  /* temporary loop variable */
//   unsigned char message[STRING_SIZE]; // this is message that server will send
   unsigned short server_secret_code; // this will hold secret_code received from client in communication
   unsigned short client_secret_code; // this will hold client's secret_code received from Client in communication   
//   unsigned char client_code[2]; // this will hold 2 byte client_secret_code
//   unsigned char server_code[2]; // this will hold 2 byte server_secret_code
    struct Messages message; // this is structure that holds message sent by server
	struct Messages receivedMessage; // this is a structure that holds message received from client
	unsigned short randomNumber=0; 
	
   /* open a socket */

   if ((sock_server = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
      perror("Server: can't open datagram socket\n");
      exit(1);
   }

   /* initialize server address information */

   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */
   server_port = SERV_UDP_PORT; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */

   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Server: can't bind to local address\n");
      close(sock_server);
      exit(1);
   }

   /* wait for incoming messages in an indefinite loop */

   printf("Waiting for incoming messages on port %hu\n\n", 
                           server_port);

   client_addr_len = sizeof (client_addr);

   /*
	* open server Info file .. here
	* TODO
   */
   for (;;) {

      bytes_recd = recvfrom(sock_server, &receivedMessage, sizeof( struct Messages), 0,
                     (struct sockaddr *) &client_addr, &client_addr_len);
      //printf("Received Sentence is: %s\n     with length %d\n\n",
       //                  sentence, bytes_recd);

      /* prepare the message to send */
	  /*
	   * Extract message received from user,
	   * check secret code
	   */
      server_secret_code = ntohs(receivedMessage.server_secret_code);
	  client_secret_code - ntohs(receivedMessage.client_secret_code);
	  printf("client_code %d\n",client_secret_code);
	  printf("server_code %d\n",server_secret_code);
	  printf("client_name %s\n",receivedMessage.name);
	  
	  if(server_secret_code == Secret_Code){
      /* make correct message */
			message.client_secret_code = htons(client_secret_code);
			message.server_secret_code = htons(Secret_Code);
			strcpy(message.name,Name);
			//write here
			writeToFile(message,receivedMessage);
	  }
	  else{
	  /* make incorrect message */
			randomNumber = (unsigned short)rand()%65535 ;
  			message.client_secret_code = htons(client_secret_code);
			message.server_secret_code = htons(randomNumber);
			message.name[0]= '\0';
	  }
	  
	  bytes_sent = sendto(sock_server, &message, sizeof(struct Messages), 0,
               (struct sockaddr*) &client_addr, client_addr_len);
   }
}
