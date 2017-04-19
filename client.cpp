#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <malloc.h>
#include <stdio.h>
#include <iso646.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <termios.h> 
#include <unistd.h> 
#include <time.h>

#define GRAY   "\x1B[100m"
#define REDD   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

pthread_mutex_t lock;
enum Color {NONE=0, RED = 1, BLUE = 2};
enum Type { BLANK=0, P1, P2,P3,P4,P5, BUG, SWORD,WALL, POINT};
const int BREADTH=10, HEIGHT=10;
char buffer[HEIGHT*BREADTH+50];
int global_matrix[BREADTH][HEIGHT]={0};
double turn_time = 2.0;
int turns_till_now=0;
time_t start;
int playernumber=-1;
int num_of_players;
clock_t start_t, end_t, total_t;
int durationofgame;

int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

void printboard(int w,int h,char* buffer)
{
    int i,j;
   int arr[h][w];
    for(i=0;i<BREADTH;i++)
{
    for(j=0;j<HEIGHT;j++)
arr[i][j]=buffer[i*BREADTH+j]-'0';


}
    int height = h*3+2;
    int width = w*4+2;
    int k;
    char str[height+1][width+1];

    for(i=0;i<height;i++)
    {
        for(j=0;j<width-1;j++)
        {
            str[i][j]=' ';
        }
        str[i][j]='\0';
    }
/*****print boundary******/
    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            if(i==0||j==0||i==height-1||j==width-1)
            str[i][j]='*';
        }
    }
/************************/  
    for(i=0;i<h;i++)
    {
        for(j=0;j<w;j++)
        {
            
            if(arr[i][j]==8)      //block
            {
                int x=i,y=j;
                for(x=3*i+1;x<=3*i+3;x++)
                {
                    for(y=4*j+1;y<=4*j+4;y++)
                    {
                        str[x][y]='B';
                        
                    }
                }
            }
            else if(arr[i][j]==9)  //coin
            {
                char coin[3][4]={{' ','_','_',' '},{'/','$','$','\\'},{'\\','$','$','/'}};
                int x=i,y=j;
                for(x=3*i+1;x<=3*i+3;x++)
                {
                    for(y=4*j+1;y<=4*j+4;y++)
                    {
                        str[x][y]=coin[x-3*i-1][y-4*j-1];
                        
                    }
                }
            }
            else if(arr[i][j]==7)  //sword
            {
                char sword[3][4]={{' ',' ',' ',' '},{'-',']','=','>'},{' ',' ',' ',' '}};
                int x=i,y=j;
                for(x=3*i+1;x<=3*i+3;x++)
                {
                    for(y=4*j+1;y<=4*j+4;y++)
                    {
                        str[x][y]=sword[x-3*i-1][y-4*j-1];
                    }
                }
            }
            else if(arr[i][j]==1||arr[i][j]==2||arr[i][j]==3||arr[i][j]==4||arr[i][j]==5) //player
            {
                char player[3][4]={{'\\','O','/',' '},{' ','|',' ',' '},{'/',' ','\\',' '}};
                int x=i,y=j;
                for(x=3*i+1;x<=3*i+3;x++)
                {
                    for(y=4*j+1;y<=4*j+4;y++)
                    {
                        str[x][y]=player[x-3*i-1][y-4*j-1];
                    }
                }
            }
            else if(arr[i][j]==100) //player with sword
            {
                char player[3][4]={{'\\','O','/','@'},{' ','|',' ',' '},{'/',' ','\\',' '}};
                int x=i,y=j;
                for(x=3*i+1;x<=3*i+3;x++)
                {
                    for(y=4*j+1;y<=4*j+4;y++)
                    {
                        str[x][y]=player[x-3*i-1][y-4*j-1];
                    }
                }
            }
            else if(arr[i][j]==6)
            {
                char bug[3][4]={{'.','V','.',' '},{'(','|',')',' '},{'*','^','*',' '}};
                int x=i,y=j;
                for(x=3*i+1;x<=3*i+3;x++)
                {
                    for(y=4*j+1;y<=4*j+4;y++)
                    {
                        str[x][y]=bug[x-3*i-1][y-4*j-1];
                    }
                }
            }

           

        }
    }

    
    for(i=0;i<width;i++){
        printf(MAG "*" RESET);
    }
    end_t=clock();
    printf("                     TIME LEFT : %f \n",(durationofgame-100*((double)(clock()-start_t) / CLOCKS_PER_SEC)));
				

    printf("\n");
    for(i=1;i<height-1;i++)
    {   
        printf(MAG "*" RESET);
        for(j=1;j<width-1;j++)
        {

            int val=arr[(i-1)/3][(j-1)/4];
            if(val==0)
            {
                printf(" ");
            }
            else if(val==1)
            {
                printf( "%s%c%s",BLU,str[i][j],RESET );
            }
            else if(val==2)
            {
                printf( "%s%c%s",REDD,str[i][j],RESET );
            }
            else if(val==3)
            {
                printf( "%s%c%s",GRN,str[i][j],RESET );
            }
            else if(val==4)
            {
                printf( "%s%c%s",WHT,str[i][j],RESET );
            }
            else if(val==5)
            {
                printf( "%s%c%s",CYN,str[i][j],RESET );
            }
            
            else if(val==6)
            {
                printf( "%s%c%s",GRN,str[i][j],RESET );
            }
            else if(val==7)
            {
                printf( "%s%c%s",YEL,str[i][j],RESET );
            }
            else if(val==8)
            {
                printf( "%s %s",GRAY,RESET );
            }
            else if(val==9)
            {
                printf( "%s%c%s",CYN,str[i][j],RESET );
            }

        }
        printf(MAG "*" RESET);
        printf("\n");
    }
    
    for(i=0;i<width;i++){
        printf(MAG "*" RESET);
    }

    printf("\n");
}

void endgame()
{
	system("clear");
	int cc=HEIGHT*BREADTH;
	int i;

	int arr[num_of_players]={0};
	for(i=0;i<num_of_players;i++)
		{
			cc+=strlen(buffer+cc)+1;
			//printf("The point of player %c : %d\n",i+'A',atoi(buffer+cc));
			arr[i]=atoi(buffer+cc);
		}
	int max=-1000000000;
	for(i=0;i<num_of_players;i++)
	{
		if(arr[i]>max)
		{
			max=arr[i];
		}
	}
	if(arr[playernumber-1]==max)
	{
			printf("\n\n\n\t\t\tCONGRATULATIONS!!!!\n\n\n\t\t\tYOU WIN WITH %d POINTS",arr[playernumber-1]);
	}
	else
	{
			printf("\n\n\n\t\t\tSORRY!!!!\n\n\n\t\t\tYOU LOOSE WITH %d POINTS",arr[playernumber-1]);
	}
	printf("--------------------------------------------------POINTS TABLE----------------------------------------------\n\n" );
		for(i=0;i<num_of_players;i++)
		{
			printf("\n                                PLAYER %c         |  POINTS  - %d\n\n",i+'A',arr[i] );
		}
}

void *print_board(void* ptr);
void *player_input(void* ptr);

int main(int argc,char* argv[])
{
    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    int clientSocket;
    char *buffer;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    pthread_mutex_init(&lock,NULL);
    // User Input
    char *userInput;
    userInput = (char *)malloc(sizeof(char)*10);
    buffer = (char *)malloc(sizeof(char)*(BREADTH*HEIGHT+5));
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = inet_addr("192.168.43.58");
    memset(serverAddr.sin_zero, '\0', sizeof
            serverAddr.sin_zero);
    addr_size = sizeof serverAddr;
    if ( connect( clientSocket, (struct sockaddr *) &serverAddr, 
                addr_size) < 0 )
    {
        printf("[ERROR] Unable to connect to server.\n");
        close( clientSocket );
        exit(0);
    }
    /*receive sign in or signup data*/
    int bytes_recv=0;
    char firstdatasent[100];
    while(bytes_recv<100)
        bytes_recv+=recv ( clientSocket, firstdatasent+bytes_recv, (100)-bytes_recv, 0);

    puts(firstdatasent);
    char inp_choice='1';
    scanf(" %c",&inp_choice);
    if(inp_choice!='1' && inp_choice!='2')
        inp_choice='1';
    memset(firstdatasent,0,sizeof(firstdatasent));
    firstdatasent[0]=inp_choice;
    send (clientSocket , firstdatasent, 100, 0);
    memset(firstdatasent,0,sizeof(firstdatasent));
    bytes_recv=0;
    while(bytes_recv<100)
        bytes_recv+=recv ( clientSocket, firstdatasent+bytes_recv, (100)-bytes_recv, 0);
    puts(firstdatasent);

    memset(firstdatasent,0,sizeof(firstdatasent));
    gets(firstdatasent);
    send (clientSocket , firstdatasent, 100, 0);


    /************************************/  
    char playernum[2];
    bytes_recv=0;
    while(bytes_recv<2)
        bytes_recv+=recv ( clientSocket, playernum+bytes_recv, (2)-bytes_recv, 0);

    playernumber=playernum[0]-'0';


    pthread_t thread1, thread2;
    int  iret1, iret2;
    memset(global_matrix,'0',sizeof(global_matrix));

    iret1 = pthread_create( &thread1, NULL, print_board, (void *)(&clientSocket) );
    iret2 = pthread_create( &thread2, NULL, player_input, (void *)(&clientSocket) );
    printf("pthread_create() for thread 1 returns: %d\n",iret1);
    printf("pthread_create() for thread 2 returns: %d\n",iret2);
    durationofgame=20000;
    start_t=-1000*CLOCKS_PER_SEC;
    while(start_t==(-1000*CLOCKS_PER_SEC));
    while((durationofgame-100*((double)(clock()-start_t) / CLOCKS_PER_SEC))>0);
    	sleep(1);
    printf("pthread cancel starting\n");
    pthread_cancel(thread1);
    pthread_cancel(thread2);
    printf("pthread cancel ending\n");
    endgame(); 
    close( clientSocket );
    return 0;
}



void* print_board(void* ptr)
{
	int count=0,flag=0;
	printf("printing\n");
	while(1)
	{	  

		int total=0;
		while(total<BREADTH*HEIGHT+50)
			total+=recv ( *((int *)ptr), buffer+total, (HEIGHT*BREADTH+50)-total, 0);
		if(flag==0)
			{
				start_t = clock();
				flag=1;
			}
		system("clear");
		printboard(BREADTH,HEIGHT,buffer);
		printf("\n\n");
		int cc=HEIGHT*BREADTH;

		num_of_players=atoi(buffer+cc);
		printf("%s\n",buffer+cc );
		printf("num_of_players=%d\n",num_of_players );
		int i;
		for(i=0;i<num_of_players;i++)
		{
			cc+=strlen(buffer+cc)+1;
			printf("The point of player %c : %d\n",i+'A',atoi(buffer+cc));
		}

		printf("You are Player %c\n",'A'+playernumber-1);
		printf("\n");
    	//strcpy(globalresultbuffer,buffer);



	}
}



void* player_input(void* ptr)
{
    char c;
    char str[2];
//printf("input\n");
while((c=getch())!='.')
{
    if(c=='w')
    {
        str[0]='w';
        str[1]=0;
    send ( *((int *)ptr), str, 2, 0);
    //printf("sent\n");
        
    }

    if(c=='s')
    {
        str[0]='s';
        str[1]=0;
    send ( *((int *)ptr), str, 2, 0);
        
    }

    if(c=='a')
    {
        str[0]='a';
        str[1]=0;
    send ( *((int *)ptr), str, 2, 0);
        
    }

    if(c=='d')
    {
        str[0]='d';
        str[1]=0;
    send ( *((int *)ptr), str, 2, 0);
        
    }
   
}
}
