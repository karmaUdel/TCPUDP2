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
#include <errno.h>			/* returns error number on errors*/

#define STRING_SIZE 1024
#define Secret_Code 4747
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
/**
 * Write to clientInfo file
 * and stdout 
 */
void writeToFile(unsigned short port,struct Messages message,struct Messages receivedMessage){
  FILE * fp;
   //int i;
   /* open the file for writing*/
   char cwd[1024];
   char file[1024];   
   if (getcwd(cwd, sizeof(cwd)) != NULL)
       snprintf(file,sizeof(file),"%s%s",cwd,"/clientInfo.txt");
   else
       snprintf(file,sizeof(file),"%s%s","/usa/karma/proj2","/clientInfo.txt");   
	   //fprintf(stdout,"file is %s\n",pwd);
   fp = fopen (file,"a+");
   fprintf(fp,"%d,%d,%s\n",port,ntohs(receivedMessage.server_secret_code),receivedMessage.name);
   /* write to command lines */
     fprintf (stdout, "------------------\n");
	   fprintf (stdout, "Port Number  %d\n",port); // this is port number
   	   fprintf (stdout, "TCP Message sent:      %d\n",ntohs(message.client_secret_code)); // this is client's secret code
	   fprintf (stdout, "TCP Message received:  %d, %d\n",ntohs(receivedMessage.client_secret_code),ntohs(receivedMessage.server_secret_code)); // this is message recived from TCP server
   	   fprintf (stdout, "UDP Message sent:      %d, %d, %s\n",ntohs(message.client_secret_code),ntohs(message.server_secret_code),message.name); // this is message sent to UDP server
	   fprintf (stdout, "UDP Message received:  %d, %d, %s\n",ntohs(receivedMessage.client_secret_code),ntohs(receivedMessage.server_secret_code),receivedMessage.name); // this is message received from Udp server
	   fprintf (stdout, "------------------\n");
   
   /* close the file*/  
   fclose (fp);

}
/**
 * check if clientInfo.txt file exists or not
 */
int cfileexists(const char * filename){
    /* try to open file to read */
    FILE *file;
    if (file = fopen(filename, "r")){
        fclose(file);
        return 1;
    }
    return 0;
}
/**
 * this function makes sure that client won't contact same server again if it contacted that server before
 * if client finds server's port in clientInfo file then return 1 meaning contacted before
 * else reurn 0
 */
int client_exists(unsigned short port) {
   char cwd[1024];
   char file[1024];   
   if (getcwd(cwd, sizeof(cwd)) != NULL)
       snprintf(file,sizeof(file),"%s%s",cwd,"/clientInfo.txt");
   else
       snprintf(file,sizeof(file),"%s%s","/usa/karma/proj2","/clientInfo.txt");   
	if( cfileexists(file)){ // check whether clientInfo exists or not 
		FILE *clientInfo = fopen(file, "r");
		char line[6 + 6 + 80];  // maximum line size
		char i;

		while (!feof(clientInfo)) {
			// copy a line from file
			for (i = 0; (line[i] = fgetc(clientInfo)) != '\n' && line[i] != EOF; i++);

			// check if the first number matches with the port
			if (atoi(line) == (int)port) {
				fclose(clientInfo);
				return 1; // port found
			}
		}

		fclose(clientInfo);
	}
	return 0; //port not found
}

/**
 * This method receives the port number
 * Based on the port number first it contact tcpserver
 * 		Tcpserver sends Server secret code, based on server's secret code, client makes message for UDP server
 * Client sends server_secret_Code to UDP server and waits for reply from udpserver
 * UDP server sends server name, along with the client and server's secret code
 * if client's secret code matches what server sent then, client saves information
 * and closes connection
 */

int client(unsigned short server_port) {

   int sock_client;  /* Socket used by client */ 

   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
   unsigned short client_port = 0;  /* Port number used by client (local port) */

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
   //unsigned short server_port = 48500;  /* Port number used by server (remote port) */
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
   unsigned short client_secret_code = Secret_Code; /*this holds client secret code in short format*/
    struct Messages message; // this is structure that holds message sent by client
	struct Messages receivedMessage; // this is a structure that holds message received from server
   
      /* open a socket */

   if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      //perror("Client: can't open stream socket");
	  return errno;
      //exit(1);
   }


   /* Note: there is no need to initialize local client address information 
            unless you want to specify a specific local port
            (in which case, do it the same way as in udpclient.c).
            The local address initialization and binding is done automatically
            when the connect function is called later, if the socket has not
            already been bound. */

   /* initialize server address information */
   if ((server_hp = gethostbyname(server_hostname)) == NULL) {
      //perror("Client: invalid server hostname");
      close(sock_client);
      return errno;
      //exit(1);
   }
   /* Clear server address structure and initialize with server address */
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
                                    server_hp->h_length);
   server_addr.sin_port = htons(server_port);
   /* initialize server address information */
   
   if (connect(sock_client, (struct sockaddr *) &server_addr, 
                                    sizeof (server_addr)) < 0) {
      //perror("Client: can't connect to server");
      close(sock_client);
      return errno;
      //exit(1);
   }
   /* make message */   
   message.client_secret_code = htons(client_secret_code);
   message.server_secret_code = htons(0); // we don't know the secret code so send 0;
   message.name_size= htons(0);
   message.name[0] = '\0'; // empty name field 
   /* send message */
   bytes_sent = send(sock_client, &message, sizeof(struct Messages), 0);

   //bytes_sent = sendto(sock_client, &message, sizeof(struct Messages), 0,(struct sockaddr *) &server_addr, sizeof (server_addr));

   /* get response from server */
   bytes_recd = recv(sock_client, &receivedMessage, sizeof(struct Messages), 0);
   //bytes_recd = recvfrom(sock_client, &receivedMessage, sizeof(struct Messages), 0,(struct sockaddr *) 0, (int *) 0);
	
	/*
	 * if _msg_received 
	 * this means server is present and we've successfully contacted that server.
	 * so decode te data and call udpclient to server further communication.
	*/
	/* decode data - message will be of form 
	 *  ----2 byte---
	 *   Client-code
	 *   server-code
	 *   name-size
	 *   name
	 * expected length will max 6 bytes;
	 */
	 /* TCP sent secret code, now client will be making message for udpserver, so make one*/
	 
  /*
  * -------------------------------------------------TCP Client Ends Here --------------------------------------------------------*
  */
   close (sock_client); // close tcp socket as we'll be contacting UDP server-socket
  /*
  * -------------------------------------------------UDP Client starts Here --------------------------------------------------------*
  */   
   /*
    * Now we need to communicate to UDP server
	* so we need udp client...
	* and message is ready to send so create udp client socket
   */
   if ((sock_client = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
      //perror("Client: can't open datagram socket\n");
      return errno;
      //exit(1);
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
      //perror("Client: can't bind to local address\n");
      close(sock_client);
	  return errno;
      //exit(1);
   }

   /* end of local address initialization and binding */

   /* initialize server address information */

   if ((server_hp = gethostbyname(server_hostname)) == NULL) {
      //perror("Client: invalid server hostname\n");
      close(sock_client);
	  return errno;
      //exit(1);
   }

   /* Clear server address structure and initialize with server address */
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
                                    server_hp->h_length);
   server_addr.sin_port = htons(server_port);

   	/* Make message */
   message.client_secret_code = htons(client_secret_code);
   message.server_secret_code = htons(ntohs(receivedMessage.server_secret_code));
   message.name_size = htons(strlen(Name));
   strcpy(message.name, Name);
   /* send message to udp server*/
   bytes_sent = sendto(sock_client, &message, sizeof(struct Messages), 0,(struct sockaddr *) &server_addr, sizeof (server_addr));

   // Wait for Reply...
   bytes_recd = recvfrom(sock_client, &receivedMessage, sizeof(struct Messages), 0,(struct sockaddr *) 0, (int *) 0);
   //Reply is received 	
   // Now decode the message
   // if secret code is matched
   if(client_secret_code == ntohs(receivedMessage.client_secret_code)){
        //strcpy(message.name, receivedMessage.name);
		//message.server_secret_code = ntohs(receivedMessage.server_secret_code);
		//message.client_secret_code = ntohs(receivedMessage.client_secret_code);
        //message.name_size = ntohs(receivedMessage.name_size);
		writeToFile(server_port,message,receivedMessage);		
   }
   //else skip and close client socket
    // save and close
	message = EmptyMessage;
	receivedMessage = EmptyMessage;
  /*
  * -------------------------------------------------UDP Client Ends Here --------------------------------------------------------*
  */
  /* close socket*/
  close (sock_client);
  return 0;
}
/**
* this is a main method which checks whether server port already been contacted or not
* if not then tries to make connection
* else skips the port number 
*/
int main(void){
  unsigned short 	i = 48000;
  for(i = 48000;i<49000;i++){
	// if client doesn't exists in client info file and client discovery doesn't give any problem
	if(client_exists(i)!=1 ){
		// if server wasn't contacted so check the port
		printf("checking port :%d\n",i);
		if(client(i)!=0){
			printf("error occur on port %d\n",i);
		}
	}
	/*  //printf("%d\n",i);
	if( client(i)!=0){
		//printf("error occur on port %d\n",i);
	}*/
	else{
		//server already been contacted so skip 
		continue;
	}
  }
  return 0;
}