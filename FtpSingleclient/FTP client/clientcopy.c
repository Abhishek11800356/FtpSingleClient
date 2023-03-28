#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

 void ReceivefromServer(int sockfd);
 void SendFileToServer(int connfd);


char fname[100];

void gotoxy(int x,int y)
{
 printf("%c[%d;%df",0x1B,y,x);
}
 
 void ReceivefromServer(int sockfd)
{
       int bytesReceived = 0;
    char recvBuff[1024];
    memset(recvBuff, '0', sizeof(recvBuff));
   	 /* Create file where data will be stored */
    	FILE *fp;
	read(sockfd, fname, 256);
	//strcat(fname,"AK");
	printf("File Name: %s\n",fname);
	printf("Receiving file...");
   	 fp = fopen(fname, "ab"); 
    	if(NULL == fp)
    	{
       	 printf("Error opening file");
         return;
    	}
    long double sz=1;
    /* Receive data in chunks of 256 bytes */
    while((bytesReceived = read(sockfd, recvBuff, 1024)) > 0)
    { 
        sz++;
        gotoxy(0,8);
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
 
 void SendFileToServer(int connfd)
{

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
		    printf("File Upload completed for id: %d\n",connfd);
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
    system("clear");
    int sockfd = 0,inp;
    struct sockaddr_in serv_addr;

    /* Create a socket first */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    /* Initialize sockaddr_in data structure */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); // port
    char ip[50] = "127.0.0.1";
    serv_addr.sin_addr.s_addr = inet_addr(ip);

    /* Attempt a connection */
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }
    printf("###################### Connection Succesfull ########################\n");
    printf("Connected with ip: %s at port no: %d\n",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port));
    
        printf("Enter 1 to DownloadFile\nEnter 2 to UploadFile\n");
	scanf("%d",&inp);
	
	if(inp==1)
	{
	 ReceivefromServer(sockfd);
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
	      printf("Enter file name you wish to send from Client : ");
              scanf("%s",&fname);
          }
        else
          {
          strcpy(fname,argv[1]);
          }
        SendFileToServer(sockfd);
       }
       else 
       {
       printf("Wrong Input\n");
       }
   
   return;
}


 
