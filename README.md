Labouré Manon
Pillot Matthieu
2G2TP6

TP2 

1. In order to generate RRQ later in the TP, we need to parse the user input 
to identify the command, the server port, and the file to be retrieved from the server. For this, we rely on the basic 
principle of handling arguments in the main function.

2. We use the getaddrinfo call as we saw it in TD, to obtain the information needed to configure our socket in the
following question.
3. We have created, using the socket() call, a placeholder for the connection to our server.
4. "Pour gettftp :
   a) After consulting the documentation, we were able to develop a function for sending RRQ (Read Request) packets. It 
is crucial to carefully adhere to the request format for the server to acknowledge it. Once the request is properly 
formatted, we send it to the server using sendto().
   b) To receive the requested data packets, we create the receiveTFTPData() function. Initially, we retrieve the data 
packets provided by the server, taking care to verify the OpCode to ensure that we are indeed receiving data and 
nothing else. We also obtain the number of the received data block. Subsequently, we create a file to store these 
received data, and we send the ACK acknowledgment to the server using the dedicated function.
   c) In order to handle a request for multiple files, we place the previous functions within a for loop that sends a 
request for each of our arguments. It is important to open the file for the first time using O_TRUNC to erase the 
file's content if it already exists. In subsequent iterations, we use O_APPEND so that data blocks are added without
erasing the previous ones.
