# TCPUDP2
This is communication between TCP and UDP client server

Commands used : ./client --> to run client
				./clientWithprint --> to run client which prints out which port is trying to communicate with
				./tcpserver& --> run tcpserver in background
				./udpserver& --> run udpserver in background
				make kill --> run makefile to kill all servers
				make clean --> run makefile to remove executables
				make all --> run makefile to compile tcpserver, udpserver and client, followed by starting both the servers in background and running the client on terminal
				make runClient --> run makefile to run client
				make runServer --> run makefile to run servers in background

Presentation of file inside zip file
 Final.zip ->

    Phase 2
        clientInfo
        serverInfo
    Phase 3
        clientInfo
        serverInfo
        client script 1 --> name is test_run_ForFailure : Contains what happens when clientWithPrint is run and gets stucks in cmmunication
        client script 2 --> name is test_run_11_30  : Contains regular run of client with output on terminal when client successfully communicated with 2 servers on 11/30, followed by another run when no new client to communicate was found
        client script 3 --> name is test_run_11_28  : Contains regular run of client with output on terminal when client successfully communicated with 2 servers on 11/28
    client.c and client executable
    tcpserver.c and tcpserver executable
    udpserver.c and udpserver execuable
    Makefile
    ReadMe

