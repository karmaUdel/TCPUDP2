/* client.c */ 
/* Programmed by Aditya Karmarkar */
/* November 11, 2017 */

#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, sendto, and recvfrom */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define STRING_SIZE 1024
#define Secret_Code 4747
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
   fp = fopen ("c:\\temp\\clientInfo.txt","a");
 
   /* write 10 lines of text into the file stream*/
       fprintf (fp, "------------------\n");
	   fprintf (fp, "Port Number  %d\n",i); // this is port number
   	   fprintf (fp, "TCP Message sent:\t   %d\n",message.client_secret_code); // this is port number
	   fprintf (fp, "TCP Message received:  %d, %d\n",receivedMessage.client_secret_code,receivedMessage.server_secret_code);
   	   fprintf (fp, "UDP Message sent:\t   %d, %d, %s\n",message.client_secret_code,message.server_secret_code,message.name); // this is port number
	   fprintf (fp, "UDP Message received:  %d, %d, %s\n",receivedMessage.client_secret_code,receivedMessage.server_secret_code,receivedMessage.name);
	   fprintf (fp, "------------------\n");
   
   /* close the file*/  
   fclose (fp);

}
int cfileexists(const char * filename){
    /* try to open file to read */
    FILE *file;
    if (file = fopen(filename, "r")){
        fclose(file);
        return 1;
    }
    return 0;
}
int main(void) {

   int sock_client;  /* Socket used by client */ 

   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
   unsigned short client_port;  /* Port number used by client (local port) */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
										
											
	/*
	* this is for part -1 where only 1 server is connected
	* so, i'm fixing the servername, this remains localhost in rest of the parts too 
	*/
   char server_hostname[STRING_SIZE] = "localhost"; /* Server's hostname */
	/*
	* Reason for fixing server_port
	* this is for part 1, client will connect only 1 server i.e my server on port 48500
	* After part 1, client will connect all servers in between 48000 to 48999,
	* then this field will be changed to some intermediate port number [48000-48999], and client will try to connect all ports which has 
	* server running on them
	*/
   unsigned short server_port = 48500;  /* Port number used by server (remote port) */
   char sentence[STRING_SIZE];  /* send message */
   char modifiedSentence[STRING_SIZE]; /* receive message */
   unsigned int msg_len;  /* length of message */
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
   //unsigned short client_secret_code; /*this holds client secret code in short format*/
   //unsigned char client_code[2]; /*this holds client secret code in char format _ this field is used in message construction in tcp and udp message*/
   //unsigned short server_secret_code; /*this holds Server secret code in short format sent by tcp server*/
   //unsigned char server_code[2]; /*this holds server secret code in char format _ this field is used in message construction - the udp message*/
   //unsigned char name[80] ;//= Name; /* holds name */
    struct Messages message; // this is structure that holds message sent by client
	struct Messages receivedMessage; // this is a structure that holds message received from server
   
      /* open a socket */

   if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Client: can't open stream socket");
      exit(1);
   }


   /* Note: there is no need to initialize local client address information 
            unless you want to specify a specific local port
            (in which case, do it the same way as in udpclient.c).
            The local address initialization and binding is done automatically
            when the connect function is called later, if the socket has not
            already been bound. */

   /* initialize server address information */

   
   if (connect(sock_client, (struct sockaddr *) &server_addr, 
                                    sizeof (server_addr)) < 0) {
      perror("Client: can't connect to server");
      close(sock_client);
      exit(1);
   }
   //server_hostname = "localhost";
   if ((server_hp = gethostbyname(server_hostname)) == NULL) {
      perror("Client: invalid server hostname");
      close(sock_client);
      exit(1);
   }

   server_port = 48500; 
   /* Clear server address structure and initialize with server address */
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
                                    server_hp->h_length);
   server_addr.sin_port = htons(server_port);



   /* initialize server address information */


   /* Clear server address structure and initialize with server address */

   /* user interface */
   
   message.client_secret_code = htons(Secret_Code);
   message.server_secret_code = 0; // we don't know the secret code so send 0;
   message.name[0] = '\0'; // empty name field 
   /* make message */
   /* send message */
   bytes_sent = sendto(sock_client, &message, sizeof(struct Messages), 0,(struct sockaddr *) &server_addr, sizeof (server_addr));

   /* get response from server */

   printf("Waiting for response from server...\n");
   //bytes_recd = recvfrom(sock_client, receivedMessage, sizeof(receivedMessage), 0,
	//		(struct sockaddr *) 0, (int *) 0) 
   bytes_recd = recvfrom(sock_client, &receivedMessage, sizeof(struct Messages), 0,
                (struct sockaddr *) 0, (int *) 0);
	
   /*
    * if_ timeOut
	* part-2 : will be expecting time out here as, there is possibility that no server is listed on current port 
	* so if timeout occurs then, reinitialize and start sending message to new port
   */
   //printf("\nThe response from server is:\n");
   //printf("%s\n\n", modifiedSentence);
	/*
	 * if _msg_received 
	 * this means server is present and we've successfully contacted that server.
	 * so decode te data and call udpclient to server further communication.
	*/
	/* decode data - message will be of form 
	 *  ----2 byte---
	 *   Client-code
	 *   server-code
	 * expected length will 4 bytes;
	 */
	 
	/* Make message */
	message.client_secret_code = ntohs(receivedMessage.client_secret_code);
    message.server_secret_code = ntohs(receivedMessage.server_secret_code);	
	strcpy(message.name, Name);
	 
  /*
  * -------------------------------------------------TCP Client Ends Here --------------------------------------------------------*
  */
   close (sock_client);
  /*
  * -------------------------------------------------UDP Client starts Here --------------------------------------------------------*
  */   
   /*
    * Now we need to communicate to UDP server
	* so we need uddp client...
	* sentence is ready to send so create udp client socket
   */
   
      if ((sock_client = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
      perror("Client: can't open datagram socket\n");
      exit(1);
   }

   /* Note: there is no need to initialize local client address information
            unless you want to specify a specific local port.
            The local address initialization and binding is done automatically
            when the sendto function is called later, if the socket has not
            already been bound. 
            The code below illustrates how to initialize and bind to a
            specific local port, if that is desired. */

   /* initialize client address information */

   client_port = 0;   /* This allows choice of any available local port */

   /* Uncomment the lines below if you want to specify a particular 
             local port: */
   /*
   printf("Enter port number for client: ");
   scanf("%hu", &client_port);
   */

   /* clear client address structure and initialize with client address */
   memset(&client_addr, 0, sizeof(client_addr));
   client_addr.sin_family = AF_INET;
   client_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* This allows choice of
                                        any host interface, if more than one 
                                        are present */
   client_addr.sin_port = htons(client_port);

   /* bind the socket to the local client port */

   if (bind(sock_client, (struct sockaddr *) &client_addr,
                                    sizeof (client_addr)) < 0) {
      perror("Client: can't bind to local address\n");
      close(sock_client);
      exit(1);
   }

   /* end of local address initialization and binding */

   /* initialize server address information */

   //printf("Enter hostname of server: ");
   //scanf("%s", server_hostname);
   if ((server_hp = gethostbyname(server_hostname)) == NULL) {
      perror("Client: invalid server hostname\n");
      close(sock_client);
      exit(1);
   }
   //printf("Enter port number for server: ");
   //scanf("%hu", &server_port);

   /* Clear server address structure and initialize with server address */
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
                                    server_hp->h_length);
   server_addr.sin_port = htons(server_port);
   // get message length
   // message was created before closing the tcpclient 
   // so just send the message.
   message.client_secret_code = htons(Secret_Code);
   message.server_secret_code = htons(message.server_secret_code);
   strcpy(message.name, Name);
   /* send message */
   bytes_sent = sendto(sock_client, &message, sizeof(struct Messages), 0,
            (struct sockaddr *) &server_addr, sizeof (server_addr));


   // Wait for Reply...
   //Reply is received 
   bytes_recd = recvfrom(sock_client, &receivedMessage, sizeof(struct Messages), 0,
                (struct sockaddr *) 0, (int *) 0);
	
   // Now decode the message
   // of secret code is matched
   if(Secret_Code == ntohs(receivedMessage.client_secret_code)){
        strcpy(message.name, receivedMessage.name);
		message.server_secret_code = ntohs(receivedMessage.server_secret_code);
		message.client_secret_code = ntohs(receivedMessage.client_secret_code);		
   }
   writeToFile(message,receivedMessage);
    // save and close
  /*
  * -------------------------------------------------UDP Client Ends Here --------------------------------------------------------*
  */
  /* close socket*/
  close (sock_client);

}
