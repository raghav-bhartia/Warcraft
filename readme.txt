All of the code has been written in C but named as a cpp file for cplusplus linker convienience

To run the server:

gcc server.cpp -pthread -o server
./server port_number
The server also requires the difficulty level and number of players to be given as input. 

To run the client:

gcc client.cpp -pthread -o client
./client server_port_number
The client will then have the option to login or signup to play the game.
