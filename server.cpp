#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <stdio.h>
#include <iso646.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>  // library to close the connection
#include <sys/types.h>

const int WIDTH=10, HEIGHT=10;
int num_of_players=2;
pthread_mutex_t lock;
int players=2;
int level=3;
time_t start;
enum Color {NONE=0, RED = 1, BLUE = 2};
enum Type { BLANK=0, P1, P2, P3, P4, P5, BUG, SWORD, WALL, POINT};
int total_points[10];
int has_sword[10];
int player_positions[10][2];
// build initial map

Type initial_map[HEIGHT][WIDTH];
Type CONFIGURATION[WIDTH*HEIGHT]=
{ 
  BLANK,   BUG, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, P4,
  BLANK,    P1, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK,
  BLANK, BLANK, BLANK, BLANK, POINT, BLANK, BLANK, BLANK, BLANK, BLANK,
  BLANK, BLANK, BLANK, BUG,     BUG, BLANK, BLANK, BLANK, BLANK, BLANK,
  BLANK, BLANK, BLANK, POINT, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK,
  BLANK, BLANK, BLANK,  WALL,  WALL, BLANK, BLANK, BLANK, BLANK, BLANK,
  BLANK, BLANK, BLANK, BLANK, BLANK, BLANK,    P2, BLANK, BLANK, BLANK,
  BLANK, BLANK, BLANK, BLANK, BLANK, BLANK,   BUG, BLANK, BLANK, BLANK,
  BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK,
  BLANK, 	P3, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, P5

 };



struct Stone{
public:
 
    Type t;
    Color c;
    //gui
 
public:
 
    Stone(){
        t=BLANK;
        c = NONE;
    }
 
 
    Stone(Stone *obj){
        t = (*obj).t;
        c = (*obj).c;
    }
 
    Stone(Type t1, Color c1){
        t = t1;
        c = c1;
    }
 
};
 
 
struct Board{
public:
 
    Stone arr[8][8];
    int p1_count;
    int p2_count;
 
public:
 
    Board(){
        for(int i=0;i<6;i++){
            for(int j=0;j<7;j++){
                arr[i][j] = Stone(BLANK, NONE);
            }
        }
        p1_count=p2_count=0;
    }
 
    Board(Board *obj){
        for(int i=0;i<6;i++){
            for(int j=0;j<7;j++){
                arr[i][j] = Stone(&((*obj).arr[i][j]));
            }
        }
        p1_count=obj->p1_count;
        p2_count=obj->p2_count;
    }
 
    int eval_map()
    {
        
        return p1_count-p2_count;

    }
    
   
};

Board main_board;

Type board[HEIGHT][WIDTH];

void* input_thread(void* ptr);
void* new_map(void* ptr);
void* send_to_all(void* array);


int main(int argc,char *argv[])
{   
  
  printf("Enter difficulty level of game: ");
  scanf("%d",&level);
   printf("Enter number of players per instance: ");
  scanf("%d",&num_of_players);

   	srand(13);
    int cc=0;
    for(int i=0;i<HEIGHT;i++)
    {
      for(int j=0;j<WIDTH;j++)
      {
      	Type temp=CONFIGURATION[cc++];
      	if(temp<=5&& temp>num_of_players)
        	initial_map[i][j]=BLANK;
    	else
    		initial_map[i][j]=temp;
      }
    }

    Board main_board = Board();
    for(int i=0;i<HEIGHT;i++)
    {
      for(int j=0;j<WIDTH;j++)
      {
        main_board.arr[i][j].t = initial_map[i][j];
        board[i][j] = initial_map[i][j];
      }
    }

    int prev_p1=0, prev_p2=0;
    has_sword[0]=has_sword[1]=has_sword[2]=has_sword[3]=has_sword[4]=0;
    bool has_weapon1=false, has_weapon2=false;
    for(int i=0;i<HEIGHT;i++)
    {
      for(int j=0;j<WIDTH;j++)
      {
        if(main_board.arr[i][j].t==P1)
        {
          player_positions[0][0]=i;
          player_positions[0][1]=j;
        }
        if(main_board.arr[i][j].t==P2)
        {
          player_positions[1][0]=i;
          player_positions[1][1]=j;
        }
        if(main_board.arr[i][j].t==P3)
        {
          player_positions[2][0]=i;
          player_positions[2][1]=j;
        }
        if(main_board.arr[i][j].t==P4)
        {
          player_positions[3][0]=i;
          player_positions[3][1]=j;
        }
        if(main_board.arr[i][j].t==P5)
        {
          player_positions[4][0]=i;
          player_positions[4][1]=j;
        }
      }
    }

    int serverSocket = 0, newSocket = 0,newSocket2 = 1;
    char *recvBuffer, *buffer;
    char *recvBuffer2;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddress[num_of_players];
    struct sockaddr_in clientAddress2,clientAddress1,clientAddress3,clientAddress4,clientAddress5;
    socklen_t addr_size[num_of_players];
    socklen_t addr_size2,addr_size1,addr_size3,addr_size4,addr_size5;
    addr_size1=sizeof(clientAddress1);
    addr_size2=sizeof(clientAddress2);
     addr_size3=sizeof(clientAddress3);
    addr_size4=sizeof(clientAddress4);
     addr_size5=sizeof(clientAddress5);
    
    int randomNumber = 0, recvBytes = 0, recvBytes2 = 0, recvNumber = 0;

    recvBuffer = (char *)malloc(sizeof(char)*10);
    recvBuffer2 = (char *)malloc(sizeof(char)*10);
    buffer = (char *)malloc(sizeof(char)*(HEIGHT*WIDTH+10));
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    int port_no=atoi(argv[1]);
    
    serverAddr.sin_port = htons(port_no);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr ( "192.168.43.58" );
    /* Set all bits of the padding field to 0 */
    memset ( serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero) );
    memset(recvBuffer, 0, 10);
    memset(buffer, 0, sizeof(buffer));

    /*---- Bind the address struct to the socket ----*/
    if ( bind ( serverSocket, ( struct sockaddr *) &serverAddr, sizeof ( serverAddr ) )  < 0)
        printf("[ERROR] Socket binding Failed. \n");
    if ( listen ( serverSocket, 5 ) ==0 )
        printf ( "Server Scocket Initiated. Listening to its clients : \n" );
    else
        printf("[ERROR] More than limit.\n");

  	int array_of_sockets[num_of_players];
    /* Accept call creates a new socket for the incoming connection */
  	int ll;
      	printf("fa");
        printf("nop:%d\n",num_of_players);
  
  
  	while(1)
  	{		
	  	for(ll=0;ll<num_of_players;ll++)
	  	{
	  	if(ll==0){
		  printf("player no.:%d\n",ll+1); 
		  
		do
	  	{
	  	if ((array_of_sockets[ll] = accept ( serverSocket, (struct sockaddr*) &clientAddress1, &addr_size1)) == -1)
		{   perror("Client ERROR");  }
			printf("new connection tried\n");

		  printf("---address   %d.%d.%d.%d\n",
	  int(clientAddress1.sin_addr.s_addr&0xFF),
	  int((clientAddress1.sin_addr.s_addr&0xFF00)>>8),
	  int((clientAddress1.sin_addr.s_addr&0xFF0000)>>16),
	  int((clientAddress1.sin_addr.s_addr&0xFF000000)>>24));
	  } while(  int((clientAddress1.sin_addr.s_addr&0xFF000000)>>24)==0 );
			char *buffer2;
			buffer2 = (char *)malloc(sizeof(char)*( 100 ));
			memset(buffer2, 0, sizeof(buffer2));
			int option;
			strcpy(buffer2,"Enter 1 for sign up\nEnter 2 for log in\n"); // 43
			int s=0;
			while(s<100)
	    	s+=send( array_of_sockets[ll], buffer2+s, 100-s, 0);
			int t=0;
			while(t<100)
	    	t+=recv( array_of_sockets[ll], buffer2+t, 100-t, 0);
	    	if(buffer2[0]=='1')
	    	{
	    		option=1;
	    	}
	    	else
	    	{
	    		option=2;
	    	}
	    	strcpy(buffer2,"Enter username<comma>password\n"); 
			s=0;
			while(s<100)
	    	s+=send( array_of_sockets[ll], buffer2+s, 100-s, 0);
	    	s=0;
			while(s<100)
	    	s+=recv( array_of_sockets[ll], buffer2+s, 100-s, 0);        	
	    	if(option==2)
	    	{
	    		FILE* accounts = fopen("abcd.csv","r");
		    	char buffer3[100];
		    	printf("%s\n",buffer2);
		    	int matched=0;
		    	while(fscanf(accounts,"%s",buffer3)!=EOF)
		    	{
		    		int commac=0,i=0,flag=0;
		    		printf("%s\n",buffer3);
		    		
		    		if(strcmp(buffer3,buffer2)==0)
		    		{
		    			matched=1;

		    			break;
		    		}
		    	}
		    	fclose(accounts);
		    	if(!matched)
		    	{
		    					    		
		    		close(array_of_sockets[ll--]);
		    		continue;
		    	}

	    	}
	    	else
	    	{
	    		FILE* accounts = fopen("abcd.csv","a");
	    		fprintf(accounts,"%s\n",buffer2);
	    		fclose(accounts);
	    	}
		  }
		  if(ll==1)
		  {

		  	printf("player no.:%d\n",ll+1); 
		  
		 do
	  {
	  	if ((array_of_sockets[ll] = accept ( serverSocket, (struct sockaddr*) &clientAddress2, &addr_size2)) == -1)
		{   perror("Client ERROR");  }
			printf("new connection tried\n");

		  printf("---address   %d.%d.%d.%d\n",
	  int(clientAddress2.sin_addr.s_addr&0xFF),
	  int((clientAddress2.sin_addr.s_addr&0xFF00)>>8),
	  int((clientAddress2.sin_addr.s_addr&0xFF0000)>>16),
	  int((clientAddress2.sin_addr.s_addr&0xFF000000)>>24));
	  } while(  int((clientAddress2.sin_addr.s_addr&0xFF000000)>>24)==0 );
			char *buffer2;
			buffer2 = (char *)malloc(sizeof(char)*( 100 ));
			memset(buffer2, 0, sizeof(buffer2));
			int option;
			strcpy(buffer2,"Enter 1 for sign up\nEnter 2 for log in\n"); // 43
			int s=0;
			while(s<100)
	    	s+=send( array_of_sockets[ll], buffer2+s, 100-s, 0);
			int t=0;
			while(t<100)
	    	t+=recv( array_of_sockets[ll], buffer2+t, 100-t, 0);
	    	if(buffer2[0]=='1')
	    	{
	    		option=1;
	    	}
	    	else
	    	{
	    		option=2;
	    	}
	    	strcpy(buffer2,"Enter username<comma>password\n"); 
			s=0;
			while(s<100)
	    	s+=send( array_of_sockets[ll], buffer2+s, 100-s, 0);
	    	s=0;
			while(s<100)
	    	s+=recv( array_of_sockets[ll], buffer2+s, 100-s, 0);        	
	    	if(option==2)
	    	{
	    		FILE* accounts = fopen("abcd.csv","r");
		    	char buffer3[100];
		    	printf("%s\n",buffer2);
		    	int matched=0;
		    	while(fscanf(accounts,"%s",buffer3)!=EOF)
		    	{
		    		int commac=0,i=0,flag=0;
		    		printf("%s\n",buffer3);
		    		
		    		if(strcmp(buffer3,buffer2)==0)
		    		{
		    			matched=1;

		    			break;
		    		}
		    	}
		    	fclose(accounts);
		    	if(!matched)
		    	{
		    					    		
		    		close(array_of_sockets[ll--]);
		    		continue;
		    	}

	    	}
	    	else
	    	{
	    		FILE* accounts = fopen("abcd.csv","a");
	    		fprintf(accounts,"%s\n",buffer2);
	    		fclose(accounts);
	    	}


		  }

		  if(ll==2){
		  printf("player no.:%d\n",ll+1); 
		  
		do
	  	{
	  	if ((array_of_sockets[ll] = accept ( serverSocket, (struct sockaddr*) &clientAddress3, &addr_size3)) == -1)
		{   perror("Client ERROR");  }
			printf("new connection tried\n");

		  printf("---address   %d.%d.%d.%d\n",
	  int(clientAddress3.sin_addr.s_addr&0xFF),
	  int((clientAddress3.sin_addr.s_addr&0xFF00)>>8),
	  int((clientAddress3.sin_addr.s_addr&0xFF0000)>>16),
	  int((clientAddress3.sin_addr.s_addr&0xFF000000)>>24));
	  } while(  int((clientAddress3.sin_addr.s_addr&0xFF000000)>>24)==0 );
			char *buffer2;
			buffer2 = (char *)malloc(sizeof(char)*( 100 ));
			memset(buffer2, 0, sizeof(buffer2));
			int option;
			strcpy(buffer2,"Enter 1 for sign up\nEnter 2 for log in\n"); // 43
			int s=0;
			while(s<100)
	    	s+=send( array_of_sockets[ll], buffer2+s, 100-s, 0);
			int t=0;
			while(t<100)
	    	t+=recv( array_of_sockets[ll], buffer2+t, 100-t, 0);
	    	if(buffer2[0]=='1')
	    	{
	    		option=1;
	    	}
	    	else
	    	{
	    		option=2;
	    	}
	    	strcpy(buffer2,"Enter username<comma>password\n"); 
			s=0;
			while(s<100)
	    	s+=send( array_of_sockets[ll], buffer2+s, 100-s, 0);
	    	s=0;
			while(s<100)
	    	s+=recv( array_of_sockets[ll], buffer2+s, 100-s, 0);        	
	    	if(option==2)
	    	{
	    		FILE* accounts = fopen("abcd.csv","r");
		    	char buffer3[100];
		    	printf("%s\n",buffer2);
		    	int matched=0;
		    	while(fscanf(accounts,"%s",buffer3)!=EOF)
		    	{
		    		int commac=0,i=0,flag=0;
		    		printf("%s\n",buffer3);
		    		
		    		if(strcmp(buffer3,buffer2)==0)
		    		{
		    			matched=1;

		    			break;
		    		}
		    	}
		    	fclose(accounts);
		    	if(!matched)
		    	{
		    					    		
		    		close(array_of_sockets[ll--]);
		    		continue;
		    	}

	    	}
	    	else
	    	{
	    		FILE* accounts = fopen("abcd.csv","a");
	    		fprintf(accounts,"%s\n",buffer2);
	    		fclose(accounts);
	    	}
		  }
		  if(ll==3){
		  printf("player no.:%d\n",ll+1); 
		  
		do
	  	{
	  	if ((array_of_sockets[ll] = accept ( serverSocket, (struct sockaddr*) &clientAddress4, &addr_size4)) == -1)
		{   perror("Client ERROR");  }
			printf("new connection tried\n");

		  printf("---address   %d.%d.%d.%d\n",
	  int(clientAddress4.sin_addr.s_addr&0xFF),
	  int((clientAddress4.sin_addr.s_addr&0xFF00)>>8),
	  int((clientAddress4.sin_addr.s_addr&0xFF0000)>>16),
	  int((clientAddress4.sin_addr.s_addr&0xFF000000)>>24));
	  } while(  int((clientAddress4.sin_addr.s_addr&0xFF000000)>>24)==0 );
			char *buffer2;
			buffer2 = (char *)malloc(sizeof(char)*( 100 ));
			memset(buffer2, 0, sizeof(buffer2));
			int option;
			strcpy(buffer2,"Enter 1 for sign up\nEnter 2 for log in\n"); // 43
			int s=0;
			while(s<100)
	    	s+=send( array_of_sockets[ll], buffer2+s, 100-s, 0);
			int t=0;
			while(t<100)
	    	t+=recv( array_of_sockets[ll], buffer2+t, 100-t, 0);
	    	if(buffer2[0]=='1')
	    	{
	    		option=1;
	    	}
	    	else
	    	{
	    		option=2;
	    	}
	    	strcpy(buffer2,"Enter username<comma>password\n"); 
			s=0;
			while(s<100)
	    	s+=send( array_of_sockets[ll], buffer2+s, 100-s, 0);
	    	s=0;
			while(s<100)
	    	s+=recv( array_of_sockets[ll], buffer2+s, 100-s, 0);        	
	    	if(option==2)
	    	{
	    		FILE* accounts = fopen("abcd.csv","r");
		    	char buffer3[100];
		    	printf("%s\n",buffer2);
		    	int matched=0;
		    	while(fscanf(accounts,"%s",buffer3)!=EOF)
		    	{
		    		int commac=0,i=0,flag=0;
		    		printf("%s\n",buffer3);
		    		
		    		if(strcmp(buffer3,buffer2)==0)
		    		{
		    			matched=1;

		    			break;
		    		}
		    	}
		    	fclose(accounts);
		    	if(!matched)
		    	{
		    					    		
		    		close(array_of_sockets[ll--]);
		    		continue;
		    	}

	    	}
	    	else
	    	{
	    		FILE* accounts = fopen("abcd.csv","a");
	    		fprintf(accounts,"%s\n",buffer2);
	    		fclose(accounts);
	    	}
		  }
		  if(ll==4){
		  printf("player no.:%d\n",ll+1); 
		  
		do
	  	{
	  	if ((array_of_sockets[ll] = accept ( serverSocket, (struct sockaddr*) &clientAddress5, &addr_size5)) == -1)
		{   perror("Client ERROR");  }
			printf("new connection tried\n");

		  printf("---address   %d.%d.%d.%d\n",
	  int(clientAddress5.sin_addr.s_addr&0xFF),
	  int((clientAddress5.sin_addr.s_addr&0xFF00)>>8),
	  int((clientAddress5.sin_addr.s_addr&0xFF0000)>>16),
	  int((clientAddress5.sin_addr.s_addr&0xFF000000)>>24));
	  } while(  int((clientAddress5.sin_addr.s_addr&0xFF000000)>>24)==0 );
			char *buffer2;
			buffer2 = (char *)malloc(sizeof(char)*( 100 ));
			memset(buffer2, 0, sizeof(buffer2));
			int option;
			strcpy(buffer2,"Enter 1 for sign up\nEnter 2 for log in\n"); // 43
			int s=0;
			while(s<100)
	    	s+=send( array_of_sockets[ll], buffer2+s, 100-s, 0);
			int t=0;
			while(t<100)
	    	t+=recv( array_of_sockets[ll], buffer2+t, 100-t, 0);
	    	if(buffer2[0]=='1')
	    	{
	    		option=1;
	    	}
	    	else
	    	{
	    		option=2;
	    	}
	    	strcpy(buffer2,"Enter username<comma>password\n"); 
			s=0;
			while(s<100)
	    	s+=send( array_of_sockets[ll], buffer2+s, 100-s, 0);
	    	s=0;
			while(s<100)
	    	s+=recv( array_of_sockets[ll], buffer2+s, 100-s, 0);        	
	    	if(option==2)
	    	{
	    		FILE* accounts = fopen("abcd.csv","r");
		    	char buffer3[100];
		    	printf("%s\n",buffer2);
		    	int matched=0;
		    	while(fscanf(accounts,"%s",buffer3)!=EOF)
		    	{
		    		int commac=0,i=0,flag=0;
		    		printf("%s\n",buffer3);
		    		
		    		if(strcmp(buffer3,buffer2)==0)
		    		{
		    			matched=1;

		    			break;
		    		}
		    	}
		    	fclose(accounts);
		    	if(!matched)
		    	{
		    					    		
		    		close(array_of_sockets[ll--]);
		    		continue;
		    	}

	    	}
	    	else
	    	{
	    		FILE* accounts = fopen("abcd.csv","a");
	    		fprintf(accounts,"%s\n",buffer2);
	    		fclose(accounts);
	    	}
		  }
	  }
			  
			  
  
  		pid_t pid = fork();
  		if(pid==0)
  		{
  			continue;
  		}
  		else if(pid==-1)
  		{
  			printf("unsuccessfull setup :(");
  			return 0;
  		}
  		else
  		{
  			break;
  		}  
}

	printf("Sockets created\n");
    int i,j;
    for(i=0;i<HEIGHT;i++)
        {
          for(j=0;j<WIDTH;j++)
          {
              int tttemp= (int)main_board.arr[i][j].t;
              buffer[i*HEIGHT + j] = (char)(tttemp + ((int)'0'));
          }
        }
     
    printf("%s",buffer);
    int lt;
   	usleep(100000);
    pthread_mutex_init(&lock,NULL);
    pthread_t thread1, thread2;
    pthread_t threads[num_of_players];
    int  iret1, iret2;
    int arr_of_ret[num_of_players];
    for(ll=0;ll<num_of_players;ll++)
    {
        arr_of_ret[ll] = pthread_create( &threads[ll], NULL, input_thread,(void*)&array_of_sockets[ll]);      
    }
    sleep(1);
    iret1 = pthread_create( &thread1, NULL, send_to_all, (void*)array_of_sockets);
    iret2 = pthread_create( &thread2, NULL, new_map, NULL);
	
    for(i=0;i<HEIGHT;i++)
        {
          for(j=0;j<WIDTH;j++)
          {
              int tttemp= (int)main_board.arr[i][j].t;
              buffer[i*HEIGHT + j] = (char)(tttemp + ((int)'0'));
          }
        }
   	int duration=200;
  	sleep(duration+5);
    for(ll=0;ll<num_of_players;ll++)
    {
        close(array_of_sockets[ll]);
    }
   
    return 0;    
}

void* input_thread(void* ptr)
{
  	printf("inp started\n");
  	pthread_mutex_lock(&lock);
  	int playerid=++players;
  	int player_id = playerid;
  	pthread_mutex_unlock(&lock);
	int socket=*((int*)ptr);
	char buffer[10];
	buffer[0]=playerid+'0';
	buffer[1]=0;
	send(socket,buffer,2,0);//sending player id
	player_id = player_id-1;

	while(1){
        recv ( socket, buffer,2, 0);  
        char inp1 = buffer[0];
        int plx, ply;
        int p1x, p1y;
        pthread_mutex_lock(&lock);
        plx = player_positions[player_id][0];
        ply = player_positions[player_id][1];
        p1x = player_positions[player_id][0];
        p1y = player_positions[player_id][1];
        int in1=0;
        if(inp1=='a')in1=1;
        else if(inp1=='w')in1=2;
        else if(inp1=='d')in1=3;
        else if(inp1=='s')in1=4;
        else in1=0;
        bool pos=false;
        // if(in1==0)pos=true;
        
        if(in1==1 && p1y==0)pos=false;
                if(in1==3 && p1y==WIDTH-1)pos=false;
                        if(in1==2 && p1x==0)pos=false;
                                if(in1==4 && p1y==HEIGHT-1)pos=false;
        
        
        
        if(in1==1)
        {
          if(p1y>=1)
          {
            if(board[p1x][p1y-1]!=WALL)
            {
              pos=true;
              // p1_future=board[p1x][p1y-1];
            }
          }
        }
        else if(in1==2)
        {
          if(p1x>=1)
          {
            if(board[p1x-1][p1y]!=WALL)
            {
              pos=true;
              // p1_future=board[p1x-1][p1y];
            }
          }
        }
        else if(in1==3)
        {
          if(p1y<=WIDTH-2)
          {
            if(board[p1x][p1y+1]!=WALL)
            {
              pos=true;
              // p1_future=board[p1x][p1y+1];
            }
          }
        }
        else if(in1==4)
        {
          if(p1x<=HEIGHT-2)
          {
            if(board[p1x+1][p1y]!=WALL)
            {
              pos=true;
              // p1_future=board[p1x+1][p1y];
            }
          }
        }
        
                if(in1==1 && p1y==0)pos=false;
                if(in1==3 && p1y==WIDTH-1)pos=false;
                        if(in1==2 && p1x==0)pos=false;
                                if(in1==4 && p1x==HEIGHT-1)pos=false;
         
        if(!pos)
        {
        	usleep(50000);
        	pthread_mutex_unlock(&lock);
        	continue;
        }
        
        if(pos)
        {
          // do the move and update the board
          printf("pid:%d\n",(player_id+1));
          int newx, newy;
          if(in1==1)
          {
            newx=plx;
            newy=ply-1;
          }
          else if(in1==2)
          {
            newx=plx-1;
            newy=ply;
          }
          else if(in1==3)
          {
            newx=plx;
            newy=ply+1;
          }
          else
          {
            newx=plx+1;
            newy=ply;
          }

          if(board[newx][newy] == BLANK)
          {
            board[newx][newy] =  (Type)playerid; 
            board[plx][ply] =BLANK;
              			player_positions[player_id][0]=newx;
			player_positions[player_id][1]=newy;
          }
          else if(board[newx][newy] == POINT)
          {
            board[newx][newy] = (Type) playerid; 
            board[plx][ply] =BLANK;
            total_points[player_id]++;
              			player_positions[player_id][0]=newx;
			player_positions[player_id][1]=newy;

          }
          else if(board[newx][newy] == BUG)
          {
            if(has_sword[player_id]==1)
            {
              board[newx][newy] = (Type) playerid; 
              board[plx][ply] =BLANK;
              total_points[player_id]++;
              has_sword[player_id]=0;  
              			player_positions[player_id][0]=newx;
			player_positions[player_id][1]=newy;

            }
            else
            {
              board[newx][newy] = (Type) playerid; 
              board[plx][ply] =BLANK;
              total_points[player_id]-=2;
              			player_positions[player_id][0]=newx;
			player_positions[player_id][1]=newy;
  
            }
            
          }
          else if(board[newx][newy] == SWORD)
          {
            board[newx][newy] = (Type)playerid; 
            board[plx][ply] =BLANK;
            has_sword[player_id]=1;
              			player_positions[player_id][0]=newx;
			player_positions[player_id][1]=newy;

          }
          else
          { 
            int opp_player_id = (int)board[newx][newy];
            if(has_sword[player_id]==1 && has_sword[opp_player_id]==0)
            {
              //board[plx][ply] = board[newx][newy];
              //board[newx][newy] = (Type)playerid;
              total_points[player_id]+=3;
              total_points[opp_player_id]-=3;
              has_sword[player_id]=0;  
              //			player_positions[player_id][0]=newx;
			//player_positions[player_id][1]=newy;

            }

            // else
            // {
            //   board[newx][newy] = player_id; 
            //   board[plx][ply] =BLANK;
            //   total_points[player_id]-=2;  
            // }
            
          }


          pthread_mutex_unlock(&lock);


          usleep(50000);

        }

}
}

void* new_map(void* ptr)
{

printf("new map started\n");
if(level==1){
while(1)
{ 
  sleep(4);
  pthread_mutex_lock(&lock);
  int i;
  int count=0;
  int max_to_be_added = 4;
  for(i=0;i<25;i++)
  {
    int rnd = rand()%(HEIGHT*WIDTH) +1;
    int x,y;
    x = rnd/WIDTH;
    y = rnd%WIDTH;
    if(board[x][y]==BLANK)
    { 
      count++;
      if(count>max_to_be_added)
      {
        break;
      }
      int type=1;
      if(rand()%2==0)
      {
        type=0;
      }
      if(rand()%2==0)
      {
        type=0;
      }
      if(rand()%2==0)
      {
        type=0;
      }
      if(rand()%2==0)
      {
        type=0;
      }
      if(type)
      {
        board[x][y]=BUG;   
      }
      else
      {
        int tt = rand()%6;
        if(tt==0)
        {
          board[x][y]=SWORD;      
        }
        else
        {
          board[x][y]=POINT;       
        }
      }
    }
    
  }


  pthread_mutex_unlock(&lock);  

}
}
else if(level==2)
{
	int listofbugs[100][2];
while(1)

	{ 
	int count=0;
	int i,j;
  sleep(4);
  pthread_mutex_lock(&lock);
 
  int max_to_be_added = 4;
  for(i=0;i<25;i++)
  {
    int rnd = rand()%(HEIGHT*WIDTH) +1;
    int x,y;
    x = rnd/WIDTH;
    y = rnd%WIDTH;
    if(board[x][y]==BLANK)
    { 
      count++;
      if(count>max_to_be_added)
      {
        break;
      }
      int type=1;
      if(rand()%2==0)
      {
        type=0;
      }
      if(rand()%2==0)
      {
        type=0;
      }
      if(rand()%2==0)
      {
        type=0;
      }
      if(rand()%2==0)
      {
        type=0;
      }
      if(type)
      {
        board[x][y]=BUG;   
      }
      else
      {
        int tt = rand()%6;
        if(tt==0)
        {
          board[x][y]=SWORD;      
        }
        else
        {
          board[x][y]=POINT;       
        }
      }
    }
    
  }

  count=0;
  for(i=0;i<HEIGHT;i++)
  {
  	for(j=0;j<WIDTH;j++)
  	{
  		if(board[i][j]==BUG)
  		{
  			listofbugs[count][0]=i;
  			listofbugs[count][1]=j;
  			count++;
  		}

  	}
  }
  for(i=0;i<count;i++)
  {
  	j=rand()%4;
  	int x=listofbugs[i][0];
  	int y=listofbugs[i][1];
  	if(j==0&&y+1<WIDTH&&board[x][y+1]==BLANK)
  	{
  		board[x][y]=BLANK;
  		board[x][y+1]=BUG;
  	}
  	if(j==1&&x-1>=0&&board[x-1][y]==BLANK)
  	{
  		board[x][y]=BLANK;
  		board[x-1][y]=BUG;
  	}
  	if(j==2&&y-1>=0&&board[x][y-1]==BLANK)
  	{
  		board[x][y]=BLANK;
  		board[x][y-1]=BUG;
  	}
  	if(j==3&&x+1<HEIGHT&&board[x+1][y]==BLANK)
  	{
  		board[x][y]=BLANK;
  		board[x+1][y]=BUG;
  	}


  }


  pthread_mutex_unlock(&lock);  

}


}

else if(level==3)
{
	int listofbugs[100][2];
	int listofplayers[10][2];
	int counter=0;
while(1)

	{ 
	int count=0;
	int count2=0;
	int i,j;
  sleep(1);
  counter=(counter+1)%4;
  pthread_mutex_lock(&lock);
  if(counter==3){
  int max_to_be_added = 3;
  for(i=0;i<25;i++)
  {
    int rnd = rand()%(HEIGHT*WIDTH) +1;
    int x,y;
    x = rnd/WIDTH;
    y = rnd%WIDTH;
    if(board[x][y]==BLANK)
    { 
      count++;
      if(count>max_to_be_added)
      {
        break;
      }
      int type=1;
      if(rand()%2==0)
      {
        type=0;
      }
      if(rand()%2==0)
      {
        type=0;
      }
      if(rand()%2==0)
      {
        type=0;
      }
      
      if(type)
      {
        board[x][y]=BUG;   
      }
      else
      {
        int tt = rand()%6;
        if(tt==0 )
        {
          board[x][y]=SWORD;      
        }
        else
        {
          board[x][y]=POINT;       
        }
      }
    }
    
  }}
  count=count2=0;;
  for(i=0;i<HEIGHT;i++)
  {
  	for(j=0;j<WIDTH;j++)
  	{
  		if(board[i][j]==BUG)
  		{
  			listofbugs[count][0]=i;
  			listofbugs[count][1]=j;
  			count++;
  		}
  		else if(board[i][j]>=1&&board[i][j]<=5)
  		{
  			listofplayers[count2][0]=i;
  			listofplayers[count2][1]=j;
  			count2++;

  		}

  	}
  }
  for(i=0;i<count;i++)
  {
  	int x=listofbugs[i][0];
  	int y=listofbugs[i][1];
  	int smallest=10000000;
  	int index;
  for(j=0;j<count2;j++)
  {
  	int x2=listofplayers[j][0];
  	int y2=listofplayers[j][1];
  	if(abs(x2-x)+abs(y2-y)<smallest)
  		index=j,smallest=abs(x2-x)+abs(y2-y);
  }		
  int player_x=listofplayers[index][0];
  int player_y=listofplayers[index][1];
  int newx,newy;
  if(player_x<x)
  	newx=x-1;
  else if(player_x>x)
  	newx=x+1;
  if(player_y<y)
  	newy=y-1;
  else if(player_y>y)
  	newy=y+1;
  if(newx<HEIGHT&&newx>=0&&newy>=0&&newy<WIDTH&&board[newx][newy]==BLANK)

  {
  	board[x][y]=BLANK;
  	board[newx][newy]=BUG;
  }



  }


  pthread_mutex_unlock(&lock);  

}


}




}


void* send_to_all(void* array)
{
char buffer[WIDTH*HEIGHT+50];
int * array1=(int*)array;
int i,j;
printf("sent_to_all started\n");
while(1)
{

usleep(50000);
for(i=0;i<HEIGHT;i++)
{
for(j=0;j<WIDTH;j++)
{
   	int tttemp= (int)board[i][j];
  	buffer[i*HEIGHT + j] = (char)(tttemp + ((int)'0'));
//printf("%d ",tttemp);
}

printf("\n");
}
int cc= HEIGHT*WIDTH;
sprintf(buffer+cc,"%d",num_of_players);
cc=strlen(buffer)+1;
for(i=0;i<num_of_players;i++)
{
	sprintf(buffer+cc,"%d",total_points[i]);
	cc+=strlen(buffer+cc)+1;
}
for(i=HEIGHT*WIDTH;i<HEIGHT*WIDTH+15;i++)
printf("%c ",buffer[i]);

buffer[cc]='\0';
printf("%s\n",buffer);
for(i=0;i<num_of_players;i++)
{
int s=0;   	
	while(s<HEIGHT*WIDTH+50)
        s+=send( *((int *)array+i), buffer+s, (HEIGHT*WIDTH+50)-s, 0);
       // printf("data_sent %d\n",s);
}

}
}
