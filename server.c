  /*TRABALHO SISTEMAS OPERACIONAIS*
  /*NOMES : ALECSANDER GESSER, CAIO BLUMER*/
  /**/
  #include <stdio.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <string.h>
  #include <dirent.h>
  #include<pthread.h>
  #include <semaphore.h>
  #include <sys/socket.h>
  #include <sys/stat.h>
  #include <sys/types.h>
    #include <netinet/in.h>

  void create_dir(int sock);
  void rmv_dir(int sock);
  int strcmpst1nl (const char * s1, const char * s2);
  void *connection_handler(void *);
  void create_file(int sock);
  void remove_file(int sock);
  void  edit_file(sock);
  void show_file(int sock);




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
    while(1)
    {
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
    }

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
      current_dir = opendir(current_dir_name); // aponta para diretorio atual

      //Send some messages to the client
      strcpy(message, "Greetings! I am your connection handler\n");
      send(sock, message, strlen(message), 0);
      read_size = read(sock, op, 1024);
      //Receive a message from client
      while( 1)
      {
        if(strcmpst1nl(op,"mkdir") == 0)
        {
          create_dir(sock);
        }
        else if (strcmpst1nl(op,"rmdir") == 0)
        {
          pthread_mutex_lock(&lock);
          rmv_dir(sock);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "mkfile") == 0)
        {
          pthread_mutex_lock(&lock);
          create_file(sock);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "rmfile") == 0)
        {
          pthread_mutex_lock(&lock);
          remove_file(sock);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op,"edit") == 0)
        {
          pthread_mutex_lock(&lock);
          edit_file(sock);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "show") == 0)
        {
          pthread_mutex_lock(&lock);
          show_file(sock);
          pthread_mutex_unlock(&lock);
        }
        else
        {
          strcpy(message, "fail");
          send(sock, message, strlen(message), 0);
        }
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

  void show_file(int sock)
  {
    FILE* fp;
    char msg[1024];
    char name[1024];
    int read_size;

    strcpy(msg,"File name to show: ");
    send(sock, msg, strlen(msg), 0 );
    read_size = read(sock, name, 1024);
    name[read_size-1] = '\0';
    fp = fopen(name, "r");
    if(fp == NULL){
      strcpy(msg,"Error on opening file");
      send(sock, msg, strlen(msg), 0 );
    }
    else
    {
      fgets(msg, 1024, fp);
      send(sock, msg, strlen(msg), 0 );
    }
    fclose(fp);

  }

  void  edit_file(int sock){
    FILE* fp;
    char msg[1024];
    char name[1024];
    int read_size;

    strcpy(msg,"File name to edit: ");
    send(sock, msg, strlen(msg), 0 );
    read_size = read(sock, name, 1024);
    name[read_size-1] = '\0';
    fp = fopen(name, "w");
    if(fp == NULL){
      strcpy(msg,"Error on opening file");
      send(sock, msg, strlen(msg), 0 );
    }
    else
    {
      strcpy(msg,"Type your text: ");
      send(sock, msg, strlen(msg), 0 );
      read_size = read(sock, name, 1024);
      name[read_size-1] = '\0';
      if(fprintf(fp, "%s\n",name) < 0)
      {
        strcpy(msg,"Edit failed: ");
        send(sock, msg, strlen(msg), 0 );
      }
      else{
        strcpy(msg,"Edit sucessed: ");
        send(sock, msg, strlen(msg), 0 );
      }
    }
    fclose(fp);
  }


  void remove_file(int sock)
  {
    char name[1024];
    char msg[1024];
    int read_size;
    strcpy(msg, "Enter file name: ");
    send(sock,msg, strlen(msg), 0);
    read_size = read(sock, name, 1024);
    name[read_size-1] = '\0';
    if ( remove(name) != 0) {
      strcpy(msg,"Failed to remove file");
      send(sock,msg,strlen(msg),0);
    }
    else
    {
      strcpy(msg,"File removed with success");
      send(sock,msg,strlen(msg),0);
    }
  }



  void create_file(int sock)
  {
    FILE* fp;
    char name[1024];
    char msg[1024];
    int read_size;
    strcpy(msg, "Enter file name: ");
    send(sock,msg, strlen(msg), 0);
    read_size = read(sock, name, 1024);
    name[read_size-1] = '\0';
    fp = fopen(name, "w");
    if (fp == NULL) {
      strcpy(msg,"Failed to create file");
      send(sock,msg,strlen(msg),0);
    }
    else
    {
      strcpy(msg,"File created with success");
      send(sock,msg,strlen(msg),0);
    }
    fclose(fp);
  }

  void create_dir(int sock)
  {
    int read_size;
    char message[1024];
    char name[1024];

    strcpy(message, "Directory name: ");
    send(sock, message, strlen(message), 0);
    read_size = read(sock, name, 1024);
    name[read_size - 1] = '\0';
    if(mkdir(name,ALLPERMS) == -1)
    {
      strcpy(message, "Error, failed to create directory");
      send(sock, message, strlen(message), 0);
    }
    else
    {
      strcpy(message, "Directory created with success!");
      send(sock, message, strlen(message), 0);
    }
    return;
  }

  void rmv_dir(int sock)
  {
    int read_size;
    char message[1024];
    char name[1024];

    strcpy(message, "Directory name: ");
    send(sock, message, strlen(message), 0);
    read_size = read(sock, name, 1024);
    name[read_size - 1] = '\0';
    if(rmdir(name) == -1)
    {
      strcpy(message, "Error, failed to remove directory");
      send(sock, message, strlen(message), 0);
    }
    else
    {
      strcpy(message, "Directory removed with success!");
      send(sock, message, strlen(message), 0);
    }
    return;

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
