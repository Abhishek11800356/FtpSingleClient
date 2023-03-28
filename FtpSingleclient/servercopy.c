#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>


struct sockaddr_in c_addr;
char fname[100];

void gotoxy(int x,int y)
 {
 printf("%c[%d;%df",0x1B,y,x);
 }

void Receivefromclient(int connfd)
{
     int bytesReceived = 0;
    char recvBuff[1024];
    memset(recvBuff, '0', sizeof(recvBuff));
      printf("Connected to Client with ip: %s at port: %d\n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
       FILE *fp;
	read(connfd, fname, 256);
	//strcat(fname,"AK");
	printf("File Name: %s\n",fname);
	printf("Receiving file...\n");
   	 fp = fopen(fname, "ab"); 
    	if(NULL == fp)
    	{
       	 printf("Error opening file");
         return 1;
    	}
    long double sz=1;
    /* Receive data in chunks of 256 bytes */
    while((bytesReceived = read(connfd, recvBuff, 1024)) > 0)
    { 
        sz++;
        gotoxy(0,11);
        printf("Received: %llf Mb",(sz/1024));
	fflush(stdout);
        // recvBuff[n] = 0;
        fwrite(recvBuff, 1,bytesReceived,fp);
        // printf("%s \n", recvBuff);
    }
    
    if(bytesReceived < 0)
    {
        printf("\n Read Error \n");
    }
    printf("\nFile Downloaded Succesfully\n");
}


void SendFileToClient(int connfd)
{
      
     // printf("Connection accepted and id: %d\n",connfd);
      printf("Connected to Client with ip: %s at port: %d\n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));

       write(connfd, fname,256);

        FILE *fp = fopen(fname,"rb");
        if(fp==NULL)
        {
            printf("File opern error");
            return;   
        }   
        /* Read data from file and send it */
        while(1)
        {
            /* First read file in chunks of 256 bytes */
            unsigned char buff[1024]={0};
            int nread = fread(buff,1,1024,fp);
            //printf("Bytes read %d \n", nread);        

            /* If read was success, send data. */
            if(nread > 0)
            {
                //printf("Sending \n");
               write(connfd, buff, nread);
            }
            if (nread < 1024)
            {
                if (feof(fp))
		{
                    printf("End of file\n");
		    printf("File transfer completed for id: %d\n",connfd);
		}
                if (ferror(fp))
                    printf("Error reading\n");
                break;
            }
     
        }
printf("Closing Connection for id: %d\n",connfd);
close(connfd);
shutdown(connfd,SHUT_WR);
sleep(2);
}

int main(int argc, char *argv[])
{
    int connfd = 0,err,inp,x=0;
    pthread_t tid; 
    struct sockaddr_in serv_addr;
    int listenfd = 0,ret;
    char sendBuff[1025];
    int numrv;
    size_t clen=0;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd<0)
	{
	  printf("Error in socket creation\n");
	  exit(2);
	}

    printf("#################### Socket created successfully #######################\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    ret=bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if(ret<0)
    {
      printf("Error in bind\n");
      exit(2);
    }
printf("****************** Binding Succesfull ******************\n");
    if(listen(listenfd, 50) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }
printf("                     <-- Listening -->                     \n");

    while(1)
    {
        clen=sizeof(c_addr);
       // printf("Waiting...\n");
        connfd = accept(listenfd, (struct sockaddr*)&c_addr,&clen);
        if(connfd<0)
        {
	  printf("Error in accept\n");
	  continue; //exit(1);	
	}
	printf("Enter 1 to ReceiveFile\nEnter 2 to SendFile\n");
	scanf("%d",&inp);
	
	if(inp==1)
	 {
	   Receivefromclient(connfd);
	 }
       else if(inp==2)
       { 
          printf("Enter 1 to view all the files present in the directory\n");
          int ls;
          scanf("%d",&ls);
            if(ls==1)
            {
            system("ls\n");
            }
          if (argc < 2) 
        {
	printf("Enter file name you wish to send from server : ");
        scanf("%s",&fname);
        
        }
        else
          {
          strcpy(fname,argv[1]);
          }
        SendFileToClient(connfd);
       }
       else 
       {
       printf("Wrong Input\n");
       }
	
        //err = pthread_create(&tid, NULL, &SendFileToClient, &connfd);
       // if (err != 0)
       //     printf("\ncan't create thread :[%s]", strerror(err));
   }
    close(connfd);
    return 0;
}
