# SimpleTCP
## Introduction 
This is an introudction to how TCP works. It's a simple project meant to write an ASCII line based TCP connection. Meaning, it will only accept a ASCII characters and will  

It consits of a server and a client part, which are communicating using IPV4. Note, the server can only handle one client at a time, however, it can queue. 

### Server
Server is supposed to be able to "recive" a connect from a client. It's supposed to send recive an 'OK\n' from the client if it can accept the protocls or 'ERROR: MISSMATCH PROTOCOL\n' if it does not. 

### Client
A client is supposed to be able to and see if it can match any protocols to the client. If it can't it's supposed to send an error message, like 'ERROR: CANT CONNECT TO <ipaddr>'.

### Test
The "test" compilation, is just to experiment with the calc library and how inputs and outputs to that library can look like. Makes it easier to understand 

## Future improvments / features. 
The client should be able to read a message from the server and compute a randomaly generated operation. The message will look like '<OPERATION> <VALUE1> <VALUE2>\n' and the operations are:
- add/div/mul/sub for these VALUE1 and VALUE2 have to be integers. 
- fadd/fdiv/fmul/fsub for these VALUE1 and VALUE2 are floating-point values. 

It will send the resulting value back to the server, where the server will double check the answer, sending back either 'OK\n', or 'ERROR\n'. after this one operatin, the client can close out and the server can handle the next client. 

The client should be able to handle the following input '<DNS|IPv4|IPv6>:<PORT>' for where to send the information.

## Author(s)
- Uplink036
- Patrik Arlos (inital repo)
