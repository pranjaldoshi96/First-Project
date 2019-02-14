// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define PORT 8080 
#define MAXCLIENT 100
int pidGedit,pidTerm,processId;


int shmid1,shmid2,shmid3;
int semid;
int *ptag, *geditTag, *tTag;

int init()
{
	shmid1= shmget(IPC_PRIVATE, MAXCLIENT*sizeof(int), 0777|IPC_CREAT);
	shmid2= shmget(IPC_PRIVATE, MAXCLIENT*sizeof(int), 0777|IPC_CREAT);
	shmid3= shmget(IPC_PRIVATE, MAXCLIENT*sizeof(int), 0777|IPC_CREAT);

	ptag = (int *) shmat(shmid1, 0, 0);
	geditTag = (int *) shmat(shmid2, 0, 0);
	tTag = (int *) shmat(shmid1, 0, 0);
	return 1;

}

int destroy()
{
	shmdt(ptag);	shmdt(geditTag);	shmdt(tTag);
	shmctl(shmid1, IPC_RMID, 0);
    shmctl(shmid2, IPC_RMID, 0);
    shmctl(shmid3, IPC_RMID, 0);
}



void handle_signal(int sig)
{
    if(sig ==SIGINT)
    {
	   // printf("TTKILL");
        kill(pidGedit,sig);
        kill(pidTerm,sig);
        destroy();
        exit(0);
    }
}
void runCommand(int fd,int *flag)
{
	int qFlag=*flag;
    *flag=0;
   
    char temp[50];
    sprintf(temp,"./compile file_%d.c 1",fd);
	FILE *fp;
	char path[1035];

	char output[10000];

    /* Open the command for reading. */
    fp = popen(temp, "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof(path)-1, fp) != NULL) {
        printf("%s", path);
	strcat(output,path);
	memset(path,0,1035);
    }


    /* close */
    pclose(fp);

    char geditKillCommand[100];
    sprintf(geditKillCommand,"ps aux |grep 'gedit -s file_%d.c'",fd);


    fp = popen(geditKillCommand , "r");
    if (fp == NULL)
    {
        printf("ERROR!\n");
    }

    int my_id = getpid();
    char parentID[100];
    char processID[100];
    char command[100];
    char t[1000];
    while (fscanf(fp, "%s %s %[^\n]%*c",t,command,t) != EOF)
    {
	    //	printf("GrepOutput : %s\n",command);
		int pid = atoi(command);
		char killC[100];
		sprintf(killC,"kill -9 %d",pid);
		system(killC);		
		printf("Killing Process with id %d\n",pid);
		break;

    }
    pclose(fp);


    char terminalKillCommand[100];
    sprintf(terminalKillCommand,"ps aux |grep 'bash ./script_%d.c'",fd);



    //Handling terminal 
    fp = popen(terminalKillCommand , "r");
    if (fp == NULL)
    {
        printf("ERROR!\n");
    }

	memset(command,'\0',100);
    char newname[100];
    sprintf(newname,"pid_%d.txt",fd);
    FILE *fd2 = fopen(newname,"r");
    int ttyid;
    fscanf(fd2,"%d",&ttyid); 
    //printf("\nTTYID :\t%d\n",ttyid);
   // while (fscanf(fp, "%s %s %[^\n]%*c",t,command,t) != EOF)
   // {
	    //	printf("GrepOutput : %s\n",command);
//		int pid = atoi(command);
		char killC[100];
		sprintf(killC,"kill -9 %d",ttyid);
		//printf("Command %s\n",killC);
		system(killC);	

//		char newKill[100];	

//		sprintf(newKill,"kill -9 %d %d",pid,getpid());
//		printf("Command %s\n",killC);
//		system(newKill);	
//		printf("Killing Process with id %d\n",pid);
//		break;

//    }
	if(qFlag!=98)
    send(fd,output,strlen(output), 0);
}
void quitCommand(int fd)
{
	int flag =98;
	runCommand(fd,&flag);
	send(fd,"@@@@",4,0);
	exit(0);
}
void createScriptFile(int fd)
{
    char filename[50];
    sprintf(filename,"script_%d.sh",fd);
    char cFilename[50];
    sprintf(cFilename,"file_%d.c",fd);
    FILE *f = fopen(filename,"w");
    if(f == NULL){
        printf("Empty file \n");
        return ;
    }
    fprintf(f,"echo $$>pid_%d.txt\nwhile : \ndo \n./compile %s 0 \nsleep 2\ndone",fd,cFilename);
    fclose(f);
    char cmd[60];
    //flagging it executable
    sprintf(cmd,"chmod +x %s",filename);
    system(cmd);
}
void createCommand(int fd,int * flag)
{
    *flag =1;
    char cmd[100];
    /*Opening Gedit as child of current process*/
    	FILE * geditFile;
	char del[100];
	sprintf(del,"file_%d.c",fd);
	geditFile = fopen(del,"w");
	fprintf(geditFile," ");
	fclose(geditFile);
	sprintf(cmd,"gedit -s file_%d.c &",fd);
        system(cmd); // to open a gedit window
printf("\nBefore create script");
	createScriptFile(fd); //creating a script file
printf("\nAfter create script");

	char name[100];
	//Opening Terminal as child of current child process
	sprintf(name,"gnome-terminal  -e 'bash ./script_%d.sh' ",fd);
        system(name);

//    pidGedit = fork();
/*    if(pidGedit==0)
    {
        geditTag[processId]=getpid();
        char cmd[50];
        char fname[50];
        sprintf(fname,"file_%d.c",fd);
        FILE *f = fopen(fname,"w");
	fclose(f);
        sprintf(cmd,"gedit -s file_%d.c &",fd);
        system(cmd); // to open a gedit window
        wait(NULL); //wait till user calls run command
    }
    else
    {
        pidTerm = fork();
        
        if(pidTerm==0)
        {
            tTag[processId]=getpid();
            char name[100];
            createScriptFile(fd); //creating a script file
            sprintf(name,"gnome-terminal  -e 'bash ./script_%d.sh' ",fd);
            system(name);
            wait(NULL);
        }
        else
        {
        }
    }
    */


}
int main(int argc, char const *argv[]) 
{ 
    processId =1;
    if(!init())
    {
        exit(0);
    }
    signal(SIGINT,handle_signal);
	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	char buffer[1024] = {0}; 
	char *hello = "Hello from server"; 
	
	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
    while(1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                        (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        } 
        processId++;
        int pid=fork();
        if(pid==0)
        {
            int flag=0;
            while((1))
            {
                valread = read( new_socket , buffer, 1024); 
                
                if(!strcmp("create\n",buffer))
                {
                    if(flag==0)
                    {
                        createCommand(new_socket,&flag);
                        send(new_socket,"\n",2,0);
                    }

                    else
                    {
                        printf("Close previous editor before opening a new one\n");
                        send(new_socket,"Close previous editor before opening a new one\n",48,0);
                    }
                    memset(buffer,'\0',sizeof(buffer));
                }
                else if(!strcmp("run\n",buffer))
                {
                    runCommand(new_socket,&flag);
                    //send(new_socket,"Run\n",5,0);
                }
                else if(!strcmp("quit\n",buffer))
                {
                    quitCommand(new_socket);
                   // send(new_socket,"\n",2,0);
                }
                else
                {
                    printf("Invalid Command by user\n");
                    send(new_socket , "Invalid Command by user\n" , 25 , 0 ); 
                }
                memset(buffer,'\0',sizeof(buffer));
                //send(new_socket , hello , strlen(hello) , 0 ); 
            }
            
     
        }
    }

	return 0; 
} 
