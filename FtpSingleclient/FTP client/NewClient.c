#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include<string.h>
#include<sys/ioctl.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<stdio.h>
#include<net/if_arp.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#define PORT 4456
#define SIZE 1024
#define FORMAT "utf-8" 
int main() {
int fp;
int client_sock;
struct sockaddr_in server_addr;
char server_ip[16];
char buffer[SIZE];
char cmd[SIZE], msg[SIZE], path[SIZE], text[SIZE], filename[SIZE];
memset(&server_addr, 0, sizeof(server_addr));
memset(buffer, 0, sizeof(buffer));
memset(cmd, 0, sizeof(cmd));
memset(msg, 0, sizeof(msg));
memset(path, 0, sizeof(path));
memset(text, 0, sizeof(text));
memset(filename, 0, sizeof(filename));
if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
{
perror("Socket creation failed");
exit(EXIT_FAILURE);
}
if (gethostname(server_ip, sizeof(server_ip)) == -1) {
perror("Failed to get server IP");
exit(EXIT_FAILURE);
}
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(PORT);
server_addr.sin_addr.s_addr = inet_addr(server_ip);
if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
perror("Connection failed");
exit(EXIT_FAILURE);
}
while (1) {
if (recv(client_sock, buffer, SIZE, 0) == -1) {
perror("Failed to receive data from server");
exit(EXIT_FAILURE);
}
sscanf(buffer, "%[^@]@%[^\n]", cmd, msg);
if (strcmp(cmd, "DISCONNECTED") == 0) {
printf("[SERVER]: %s\n", msg);
break;
} 
else if (strcmp(cmd, "OK") == 0) {
printf("%s\n", msg);
}
memset(buffer, 0, sizeof(buffer));
memset(cmd, 0, sizeof(cmd));
memset(msg, 0, sizeof(msg));
memset(path, 0, sizeof(path));
memset(text, 0, sizeof(text));
memset(filename, 0, sizeof(filename));
fgets(buffer, SIZE, stdin);
sscanf(buffer, "%s %s", cmd, path);
if (strcmp(cmd, "HELP") == 0) {
send(client_sock, cmd, strlen(cmd), 0);
} 
else if (strcmp(cmd, "LOGOUT") == 0) {
send(client_sock, cmd, strlen(cmd), 0);
break;
}
else if (strcmp(cmd, "LIST") == 0) {
send(client_sock, cmd, strlen(cmd), 0);
} 
else if (strcmp(cmd, "DELETE") == 0) {
sprintf(buffer, "%s@%s", cmd, path);
send(client_sock, buffer, strlen(buffer), 0);
} 
else if (strcmp(cmd, "UPLOAD") == 0) {
FILE *fp = fopen(path, "r");
}
if (fp == NULL) {
perror("Failed to open file");
continue;
}
fread(text, 1, sizeof(text), fp);
fclose(fp);
strcpy(filename, strrchr(path, '/') + 1);
sprintf(buffer, "%s@%s@%s", cmd, filename, text);
send(client_sock, buffer, strlen(buffer), 0);
}
}
