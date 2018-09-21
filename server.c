/*TRABALHO SISTEMAS OPERACIONAIS*
/*NOMES : ALECSANDER GESSER, CAIO BLUMER*/
/**/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>

int strcmpst1nl (const char * s1, const char * s2);
void *connection_handler(void *);
pthread_mutex_t lock;

int main(){
  int welcomeSocket, newSocket;
  char buffer[1024];
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
  pthread_t thread_id;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(8080);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  /*---- Bind the address struct to the socket ----*/
  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  if (pthread_mutex_init(&lock, NULL) != 0)
   {
       printf("\n mutex init failed\n");
       return 1;
   }

  /*---- Listen on the socket, with 5 max connection requests queued ----*/
  if(listen(welcomeSocket,5)==0)
    printf("Listening\n");
  else
    printf("Error\n");

  /*---- Accept call creates a new socket for the incoming connection ----*/
  addr_size = sizeof serverStorage;

  while(newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size))
  {
    puts("Connection accepted");
    if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &newSocket) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Handler assigned");
    }

    if (newSocket < 0)
    {
        perror("accept failed");
        return 1;
    }
    pthread_mutex_destroy(&lock);

    return 0;


  /*---- Send message to the socket of the incoming connection ----*/
  //strcpy(buffer,"Hello World\n");
  //send(newSocket,buffer,13,0);

}

void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    DIR *current_dir = NULL;
    char current_dir_name[1024];
    int sock = *(int*)socket_desc;
    int read_size;
    char op[1024] = {0};
    char message[1024] , client_message[1024];
    getcwd(current_dir_name, sizeof(current_dir_name));
    current_dir = opendir(current_dir_name);

    //Send some messages to the client
    strcpy(message, "Greetings! I am your connection handler\n");
    send(sock, message, strlen(message), 0);
    read_size = read(sock, op, 1024);
    //Receive a message from client
    while( 1)
    {
      if(strcmpst1nl(op,"teste") == 0)
      {
        strcpy(message, "success");
        send(sock, message, strlen(message), 0);
      }
      else{ strcpy(message, "fail");
          send(sock, message, strlen(message), 0);}

      read_size = read(sock, op, 1024);

    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

}

int strcmpst1nl (const char * s1, const char * s2)                              //precisa trocar essa função pq é igual a do claudio
{                                                                               //
  char s1c;                                                                     //
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
