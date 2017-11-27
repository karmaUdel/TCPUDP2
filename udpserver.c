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

#define SERV_UDP_PORT 48004
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

void writeToFile(struct Messages message,struct Messages receivedMessage){
  FILE * fp;
   int i;
   /* open the file for writing*/
   char cwd[1024];
   char file[1024];   
   if (getcwd(cwd, sizeof(cwd)) != NULL)
       snprintf(file,sizeof(file),"%s%s",cwd,"/serverInfo.txt");
   else
       snprintf(file,sizeof(file),"%s%s","/usa/karma/proj2","/serverInfo.txt");
	//fprintf(stdout,"file is %s\n",pwd);
   fp = fopen (file,"a+");
   //write to file
   fprintf(fp,"%d,%s\n",ntohs(receivedMessage.client_secret_code),receivedMessage.name);
   
   /* write 10 lines of text into the file stream*/
   /*    fprintf (fp, "------------------\n");
	   fprintf (fp, "Port Number  %d\n",i); // this is port number
   	   fprintf (fp, "TCP Message sent:\t   %d\n",ntohs(message.client_secret_code)); // this is port number
	   fprintf (fp, "TCP Message received:  %d, %d\n",ntohs(receivedMessage.client_secret_code),ntohs(receivedMessage.server_secret_code));
   	   fprintf (fp, "UDP Message sent:\t   %d, \%d, %s\n",ntohs(message.client_secret_code),ntohs(message.server_secret_code),message.name); // this is port number
	   fprintf (fp, "UDP Message received:  %d, %d, %s\n",ntohs(receivedMessage.client_secret_code),ntohs(receivedMessage.server_secret_code),receivedMessage.name);
	   fprintf (fp, "------------------\n");
   */
   /* close the file*/  
   fclose (fp);

}

int getmessageSize(struct Messages message){
	int length = 0;
	
	length =  sizeof(message.client_secret_code) +  sizeof(message.server_secret_code) + sizeof(message.name_size) + (int)ntohs(message.name_size);
	return length;
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
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
   unsigned int i;  /* temporary loop variable */
   unsigned short server_secret_code = Secret_Code; // this will hold secret_code received from client in communication
   unsigned short client_secret_code; // this will hold client's secret_code received from Client in communication   
    struct Messages message; // this is structure that holds message sent by server
	struct Messages receivedMessage; // this is a structure that holds message received from client
	unsigned short randomNumber=0; 
	int successFlag = 0; // 0 means failure, 1 means success
	
   /* open a socket */

   if ((sock_server = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
      //perror("Server: can't open datagram socket\n");
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
      //perror("Server: can't bind to local address\n");
      close(sock_server);
      exit(1);
   }

   /* wait for incoming messages in an indefinite loop */

   //printf("Waiting for incoming messages on port %hu\n\n", 
    //                       server_port);

   client_addr_len = sizeof (client_addr);

   /*
	* wait for incoming requests
   */
   for (;;) {
	  receivedMessage = EmptyMessage;
      bytes_recd = recvfrom(sock_server, &receivedMessage, sizeof(struct Messages), 0, (struct sockaddr *) &client_addr, &client_addr_len); //expect full 86 bytes
      if(bytes_recd < 0){
		continue;
	  }
      /* prepare the message to send */
	  /*
	   * Extract message received from user,
	   * check secret code
	   */
      //server_secret_code = ntohs(receivedMessage.server_secret_code);
	  client_secret_code = ntohs(receivedMessage.client_secret_code);
	  //printf("client_code %d\n",client_secret_code);
	  //printf("server_code %d\n",server_secret_code);
	  //printf("client_name %s\n",receivedMessage.name);
	  
	  if(server_secret_code == ntohs(receivedMessage.server_secret_code)){
      /* make correct message */
			message.client_secret_code = htons(client_secret_code);
			message.server_secret_code = htons(server_secret_code);
			message.name_size = htons(strlen(Name));
			message.name[0]= '\0'; // make empty string
			strcpy(message.name,Name); // then copy name
			//write here
			//printf("writing to file now");
			successFlag = 1;
	  }
	  else{
	  /* make incorrect message */
			randomNumber = (unsigned short)rand()%65535 ;
  			message.client_secret_code = htons(client_secret_code);
			message.server_secret_code = htons(randomNumber);
			message.name_size = htons(0);
			message.name[0]= '\0';
	  }
	  if(successFlag == 1){
			// success 
			// write to file
			writeToFile(message,receivedMessage);
			// reset the flag
			successFlag = 0;
		}
	  bytes_sent = sendto(sock_server, &message, getmessageSize(message), 0,(struct sockaddr*) &client_addr, client_addr_len); //send limited bytes
		if(bytes_sent<0){
			continue;
		}
		message = EmptyMessage;
		receivedMessage = EmptyMessage;
   }
}
