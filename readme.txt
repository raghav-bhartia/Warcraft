All of the code has been written in C but named as a cpp file for cplusplus linker convienience

To run the server:

On line 213 in server.cpp, please edit the ip address of the server accordingly.
gcc server.cpp -pthread -o server
./server port_number
The server also requires the difficulty level and number of players to be given as input. 

To run the client:
On line 309 in client.cpp, please edit the ip address of server same as above.
gcc client.cpp -pthread -o client
./client server_port_number
The client will then have the option to login or signup to play the game.
