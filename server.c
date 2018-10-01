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
  #include <dirent.h>
  #define porta 8888

  int strcmpst1nl (const char * s1, const char * s2);
  void *connection_handler(void *welcomeSocket);

  // operations
  void create_file(int sock, char* name);
  void remove_file(int sock, char* name);
  void edit_file(int sock, char* name);
  void show_file(int sock, char* name);
  void cd_directory(int sock, DIR* current_dir, char* name);
  void list_directory(int sock);
  void create_dir(int sock, char* name);
  void rmv_dir(int sock, char* name);

  pthread_mutex_t lock;

  int main(){

    int welcomeSocket, newSocket, *new_socket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in client;

    socklen_t addr_size;

    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    welcomeSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(welcomeSocket == -1)
    {
      printf("Error - Create socket");
    }

    /*---- Configure settings of the server address struct ----*/
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
    /* Set port number, using htons function to use proper byte order */
    serverAddr.sin_port = htons(porta);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr =INADDR_ANY;
    /* Set all bits of the padding field to 0 */
    memset(serverAddr.sin_zero, '0', sizeof serverAddr.sin_zero);

    /*---- Bind the address struct to the socket ----*/
    if(bind(welcomeSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
      printf("Error: Bind");
      return 1;
    }

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
        addr_size = sizeof (struct sockaddr_in);

        while(newSocket = accept(welcomeSocket, (struct sockaddr *) &client, (socklen_t*)&addr_size))
        {
          struct in_addr ipAddr = client.sin_addr;
          char ip[INET_ADDRSTRLEN];
          inet_ntop(AF_INET, &ipAddr, ip, INET_ADDRSTRLEN);

          puts("Connection accepted");
          pthread_t thread_id;
          new_socket = (int*)malloc(sizeof(int));
          *new_socket = newSocket;


          if( pthread_create( &thread_id, NULL ,  connection_handler , (void*)new_socket) < 0)
              {
                  perror("could not create thread");
                  return 1;
              }
              puts("Handler assigned");
        }
        if (new_socket < 0)
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

  void *connection_handler(void *welcomeSocket)
  {
      //Get the socket descriptor
      DIR *current_dir = NULL;
      char current_dir_name[1024];
      int sock = *(int*)welcomeSocket;
      int read_size;
      char *op, *name;
      char message[1024] , client_message[1024] = {'\0'};
      getcwd(current_dir_name, sizeof(current_dir_name));
      current_dir = opendir(current_dir_name); // aponta para diretorio atual

      //Send some messages to the client
      strcpy(message, "Greetings! I am your connection handler");
      send(sock, message, strlen(message), 0);
      read_size = read(sock, client_message, 1024);

      op = strtok(client_message," \0");
      name = strtok(NULL," \0");

      if(strcmpst1nl(op,"ls") == 0){
          name = "default";
          //printf("entrou if op==ls\n" );
      }else if(strcmpst1nl(op,"exit") == 0){name = "default";}
      else
      {
      //  printf("entrou else ls\n");
        name[strlen(name)-1] = '\0';
      }
      printf("%s\n",op );
      printf("%s\n",name );
      //Receive a message from client
      while( 1)
      {
        if(strcmpst1nl(op,"mkdir") == 0)
        {
          create_dir(sock, name);
        }
        else if (strcmpst1nl(op,"rmdir") == 0)
        {
          pthread_mutex_lock(&lock);
          rmv_dir(sock, name);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "mkfile") == 0)
        {
        //  pthread_mutex_lock(&lock);
          create_file(sock, name);
        //  pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "rmfile") == 0)
        {
          pthread_mutex_lock(&lock);
          remove_file(sock, name);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op,"edit") == 0)
        {
          pthread_mutex_lock(&lock);
          edit_file(sock, name);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "show") == 0)
        {
          pthread_mutex_lock(&lock);
          show_file(sock, name);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op,"cd") == 0)
        {
        //  pthread_mutex_lock(&lock);
          cd_directory(sock, current_dir, name);
        //  pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "ls") == 0)
        {
        //  pthread_mutex_lock(&lock);
          list_directory(sock);
        //  pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "exit") == 0)
        {
          close(sock);
          pthread_kill(pthread_self(), 0);
        }else
        {
          strcpy(message, "fail");
          send(sock, message, strlen(message), 0);
        }
        memset(client_message, '\0', 1024);
        read_size = read(sock, client_message, 1024);
        op = strtok (client_message," \0");
        name = strtok (NULL," \0");
        if(strcmpst1nl(op,"ls") == 0){
            name = "default";
            //printf("entrou if op==ls\n" );
        }else if(strcmpst1nl(op,"exit") == 0){name = "default";}
        else
        {
        //  printf("entrou else ls\n");
          name[strlen(name)-1] = '\0';
        }


        printf("%s\n",op );
        printf("%s\n",name );
      }
}

  void list_directory(int sock)
  {
    char msg[1024] = "";
    char buf[1024] = "";
    char current[1024] = "";
    getcwd(current,sizeof(current));
    struct dirent *dir = NULL;
    DIR* current_dir = opendir(current);
    dir = readdir(current_dir);
    memset(msg, 0, sizeof(msg));
    while(dir = readdir(current_dir)){
      strcat(msg, dir->d_name);
      strcat(msg, "\n");
    }
    rewinddir(current_dir);
    send(sock, msg, strlen(msg), 0);
}

  void cd_directory(int sock, DIR* current_dir, char* name)
  {
    FILE* fp;
    char msg[1024];
    int read_size;
    char current[1024];

    if(chdir(name) == -1)
    {
      strcpy(msg,"Error changing directory");
      send(sock, msg, strlen(msg), 0 );
    }
    else
    {
      getcwd(current, sizeof(current));
      current_dir = opendir(current);
      strcpy(msg, "Success!");
      send(sock, msg, strlen(msg), 0);
    }
  }

  void show_file(int sock, char* name)
  {
    FILE* fp;
    char msg[1024];
    int read_size;

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

  void  edit_file(int sock, char* name){
    FILE* fp;
    char msg[1024];
    int read_size;
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


  void remove_file(int sock, char* name)
  {
    char msg[1024];
    int read_size;
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



  void create_file(int sock, char* name)
  {
    FILE* fp;
    char msg[1024];
    int read_size;
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

  void create_dir(int sock, char* name)
  {
    int read_size;
    char message[1024];
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

  void rmv_dir(int sock, char* name)
  {
    int read_size;
    char message[1024];
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
    if( s1 == NULL) return -1;
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
