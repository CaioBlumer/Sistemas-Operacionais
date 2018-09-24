/*TRABALHO SISTEMAS OPERACIONAIS*
/*NOMES : ALECSANDER GESSER, CAIO BLUMER*/
/**/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LEN 1024

void f_send_message(int sock);
int strcmpst1nl (const char * s1, const char * s2);

int main(){
  int clientSocket;
  char buffer[1024];
  char msg[1024];
  int read_size;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);

  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(8080);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  /*---- Connect the socket to the server using the address struct ----*/
  addr_size = sizeof serverAddr;
  if(connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size)>0)
  {
    printf("Connection failed \n");
    return -1;
  }

  /*---- Read the message from the server into the buffer ----*/

  /*---- Print the received message ----*/

  while(1)
  {
  read_size = recv(clientSocket, buffer, 1024,0);
  printf("Data received: %s\n",buffer);
  fgets(msg, LEN, stdin);
  send(clientSocket, msg, strlen(msg),0);
  memset(buffer, '\0', strlen(buffer)); 
  }
 close(clientSocket);
  return 0;
}

int strcmpst1nl (const char * s1, const char * s2)
{
  char s1c;
  do
    {
      s1c = *s1;
      if (s1c == '\n')
          s1c = 0;
      if (s1c != *s2)
          return 1;
      s1++;
      s2++;
    } while (s1c);
  return 0;
}
