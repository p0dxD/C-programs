#include "hw5.h"


struct epoll_event events[MAXLINE];/*stores clients*/
int verbose = 0;
volatile int number_of_users = 0;
volatile int comm_created = 0;
char file_path[MAXLINE];
Users *head = NULL;
Accts *accounts_head = NULL;
char message_of_the_day[MAXLINE];
int main(int argc, char **argv) {

  char *port;
  if (argc < 3 || argc > 5) {
    fprintf(stderr, "%s\n", "usage: ./server [-h|-v] PORT_NUMBER MOTD");
    exit(0);
  }


  int opt;
  while ((opt = getopt(argc, argv, "vh")) != -1) {

    switch (opt) {
    case 'v':
      // fprintf(stderr, "%s\n", "verbose");
      verbose = 1;
      break;
    case 'h':
      printHelp();
      exit(0);
      break;
    default:
      exit(0);
      break;
    }
  }

  /*check the position of the arguments*/
  if (argc == 4) {/*if it has a glad*/
    strcpy(message_of_the_day, argv[3]);
    port = argv[2];
  } else { /*if its without a flag*/
    strcpy(message_of_the_day, argv[2]);
    port = argv[1];
  }

  read_accounts(NULL);
  /*print initial message IP*/
  fprintf(stderr, "\x1B[1;37mCurrently Listening on \x1B[1;36m%s\x1B[0m\n", port);
  get_ip();


  int listenfd, *connfd;
  char buf[MAXLINE];/*where the messages will be stored*/
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  fd_set read_set, ready_set;
  pthread_t tid;

  listenfd = Open_listenfd(port);


  FD_ZERO(&read_set);/*clear the set*/
  FD_SET(STDIN_FILENO, &read_set);/*add file descriptor*/
  FD_SET(listenfd, &read_set);

  // printf("FD %d\n", listenfd);
  if (Signal(SIGINT, ctrl_c_handler) == SIG_ERR)
    fprintf(stderr, "\ncan't catch SIGINT\n");
  /*The setsockopt alllows us to terminate and start connections fast*/
  while (1) {
    clientlen = sizeof(struct sockaddr_storage);
    ready_set = read_set;
    Select(listenfd + 1, &ready_set, NULL, NULL, NULL);
    if (FD_ISSET(STDIN_FILENO, &ready_set)) {
      if (!Fgets(buf, MAXLINE, stdin))
        exit(0);
      if (strstr(buf, "/users") != NULL)
        printUsers();
      else if (strstr(buf, "/help") != NULL)
        printHelp();
      else if (strstr(buf, "/accts") != NULL)
        print_accounts();
      // fprintf(stderr, "%s\n", "usage: ./server [-h|-v] PORT_NUMBER MOTD");
      else if (strstr(buf, "/shutdown") != NULL) {
        serverShutdown();
        exit(0);/*exit close fd etc*/
      } else {
        fprintf(stderr, "%s\n", "Command not found.");
      }
      fflush(STDIN_FILENO);
    }/*stdin*/
    if (FD_ISSET(listenfd, &ready_set)) {

      connfd = Malloc(sizeof(int));
      *connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);/*accept connection*/
      void *login_thread_sec(void *vargp);
      Pthread_create(&tid, NULL, login_thread_sec, connfd);

    }/*end listening message*/

  }

}


void *login_thread_sec(void *vargp) {

  int connfd = *((int*)vargp);
  Pthread_detach(pthread_self());
  Free(vargp);
  char message[MAXLINE];
  char *iam[3];
  memset(message, 0, MAXLINE);
  // read(connfd, message, strlen(WOLFIE));
  read_stuff(message, connfd);
  if (verbose == 1)
    fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);
  if (strstr(message, WOLFIE) != NULL) {
    // pause();
    write(connfd, WOLFIE_BACKWARDS, strlen(WOLFIE_BACKWARDS));
    memset(message, 0, strlen(message) + 1);

    // read(connfd, message, MAXLINE);/*read next message*/
    read_stuff(message, connfd);
    if (verbose == 1)
      fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);
    /**/


    tokenize(message, iam);
    char name_user[MAXLINE];
    strcpy(name_user, iam[1]);

    if (strcmp(iam[0], IAM_MESSAGE) == 0) {/*TRY TO LOGIN EXISTING*/

      if (!checkUser(&head, iam[1])) {/*CHECK IF USER IS ALREADY LOGGED IN*/
        memset(message, 0, strlen(message));
        reply_message(AUTH, message, name_user);/*create message*/
        write(connfd, message, strlen(message)); /*send*/

        memset(message, 0, strlen(message));
        // read(connfd, message, MAXLINE);/*read password*/
        read_stuff(message, connfd);
        if (verbose == 1)
          fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);
        tokenize(message, iam);
        //printf("password %s\n", iam[1]);
        int success = 0;
        Accts *temp = accounts_head;
        char retrievedSalt[MAXLINE];
        char saltyPass[MAXLINE];
        char buf[65];

        for (; temp; temp = temp->next_acct) {
          if (!strcmp(temp->name, name_user)) {
            //printf("Pass: %s Salt: %d\n", tokens[1], salt);
            //sprintf(saltyPass, "%s%d", iam[1], retrievedSalt);
            strcpy(retrievedSalt, temp->saltValue);
            sprintf(saltyPass, "%s%s", iam[1], retrievedSalt);
            // printf("Password + salt: %s\n", saltyPass);
            // printf("Retrieved salt: %s\n", retrievedSalt);
            sha256(saltyPass, buf);
            // printf("Hashed Password: %s\n", temp->hashedPass);
            // printf("New hash: %s\n", buf);

            if (!strcmp(temp->hashedPass, buf)) {
              // printf("%s\n", "found");
              success  = 1;
              break;
            }
          }
        }

        if (success) {
          // printf("%s\n", "successs");
          /*add to account and to users*/
          Accts *test = Malloc(sizeof(struct Accts));
          /**/
          strcpy(test->name, name_user);
          strcpy(test->saltValue, retrievedSalt);
          strcpy(test->hashedPass, buf);
          addAcct(&accounts_head, test);

          Users *test1 = Malloc(sizeof(struct Users));
          strcpy(test1->name, name_user);
          test1 -> socketfd = connfd;
          test1 -> ip = 0;
          test1 -> timeConnected = time(NULL);
          if (addUser(&head, test1)) {
            number_of_users++;
            addevent(connfd);/*add event*/
          }

          write(connfd, PASS_BACKWARDS, strlen(PASS_BACKWARDS)); /*send err*/
          memset(message, 0, MAXLINE);
          reply_message(HI_MESSAGE, message, name_user);/*create message*/
          // printf("reply_message %s\n", message);
          write(connfd, message, strlen(message));/*write back hi*/
          memset(message, 0, MAXLINE);
          reply_message("MOTD", message, message_of_the_day);/*create message*/
          write(connfd, message, strlen(message));/*write back hi*/
          // printf("%s\n", message);
          // /***********************************************/
          // /***********************************************/
          // pause();
          // /***********************************************/
          // /***********************************************/

        } else {
          write(connfd, ERR_BAD_PASSWORD, strlen(ERR_BAD_PASSWORD) + 1); /*send err*/
          write(connfd, BYE_MESSAGE, strlen(BYE_MESSAGE) + 1); /*send bye*/
          Close(connfd);/*close connection*/
          pthread_exit(NULL);
          return NULL;
        }/**/


      } else { /**/
        write(connfd, ERR_MESSAGE_USER_TAKEN, strlen(ERR_MESSAGE_USER_NOT_AVAILABLE)); /*send err*/
        write(connfd, BYE_MESSAGE, strlen(BYE_MESSAGE)); /*send bye*/
        Close(connfd);/*close connection*/
        pthread_exit(NULL);
        return NULL;
      }

    } else if (strcmp(iam[0], IAMNEW) == 0) {/*ELSE NEW USER*/
      // printf("INSIDE IAMNEW \n");
      if (!checkAcct(&accounts_head, iam[1])) {
        // char name_user[MAXLINE];
        // strcpy(name_user, iam[1]);
        // printf("%s\n", iam[1]);
        memset(message, 0, strlen(message) );
        reply_message(HINEW, message, iam[1]);/*create message*/
        write(connfd, message, strlen(message));
        // printf("Wrote %s\n", message);
        memset(message, 0, strlen(message));
        // read(connfd, message, MAXLINE);/*read password info message*/
        /*wait for password*/
        read_stuff(message, connfd);
        if (verbose == 1)
          fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);
        // printf("PASS GOTEN%s\n", message);


        char *tokens[10];
        tokenize(message, tokens);
        int lenght = 0, upper = 0, special = 0, number = 0;
        if ((strlen(tokens[1]) >= 5)) {
          lenght = 1;
          int p;
          for (p = 0; p < strlen(tokens[1]); p++) {
            if (isupper(tokens[1][p])) {
              upper = 1;
            }
            if (!isalpha(tokens[1][p])) {
              if (isdigit(tokens[1][p])) {
                number = 1;
              } else {
                special = 1;
              }
            }
          }

        } else {/*WE EXIT BECAUSE BAD PASSWORD*/
          // printf("%s\n", "bad password");
          write(connfd, ERR_BAD_PASSWORD, strlen(ERR_BAD_PASSWORD)); /*send err*/
          write(connfd, BYE_MESSAGE, strlen(BYE_MESSAGE)); /*send bye*/
          Close(connfd);/*close connection*/
          pthread_exit(NULL);
          return NULL;
          // printf("%s\n", "closed conexion");
        }
        // lenght = upper + special + number;
        // printf("lenght %d\n", lenght);
        if ((lenght + upper + special + number) == 4) {
          Accts *test = Malloc(sizeof(struct Accts));
          time_t t;
          srand((unsigned) time(&t));
          int saltGen = rand() % sizeof(tokens[1]);
          char salt[MAXLINE];
          char buf[65];
          char saltedPass[MAXLINE];


          sprintf(salt, "%d", saltGen);
          sprintf(saltedPass, "%s%s", tokens[1], salt);

          // printf("On creation Pass: %s and salt: %s\n", tokens[1], salt);

          sha256(saltedPass, buf);
          // printf("On creation hash: %s\n", buf);

          strcpy(test->name, name_user);
          strcpy(test->saltValue, salt);
          strcpy(test->hashedPass, buf);
          addAcct(&accounts_head, test);

          Users *test1 = Malloc(sizeof(struct Users));
          strcpy(test1->name, name_user);
          test1 -> socketfd = connfd;
          test1 -> ip = 0;
          test1 -> timeConnected = time(NULL);
          if (addUser(&head, test1)) {
            number_of_users++;
            addevent(connfd);/*add event*/
          }
          write_accounts(file_path);
          write(connfd, "SSAPWEN \r\n\r\n", strlen("SSAPWEN \r\n\r\n")); /*send bye*/

          // pause();
          memset(message, 0, MAXLINE);
          reply_message(HI_MESSAGE, message, name_user);/*create message*/
          // printf("reply_message %s\n", message);
          write(connfd, message, strlen(message));/*write back hi*/
          memset(message, 0, MAXLINE);
          reply_message("MOTD", message, message_of_the_day);/*create message*/
          write(connfd, message, strlen(message));/*write back hi*/
          // printf("%s\n", message);

          // pause();
        } else {
          write(connfd, ERR_MESSAGE_USER_TAKEN, strlen(ERR_MESSAGE_USER_NOT_AVAILABLE) ); /*send err*/
          write(connfd, BYE_MESSAGE, strlen(BYE_MESSAGE)); /*send bye*/
          Close(connfd);/*close connection*/
          pthread_exit(NULL);
          return NULL;
        }


        // printf("%s %s\n", "BEFORE PAUSE", message);
        // pause();
        // printf("%s\n", message);
        if (verbose == 1)
          fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);

      } else { /*ELSE ITS IN ACCOUNT SEND ERR*/
        // printf("%s\n", "inside else");
        write(connfd, ERR_MESSAGE_USER_NOT_AVAILABLE, strlen(ERR_MESSAGE_USER_NOT_AVAILABLE)); /*send err*/
        write(connfd, "BYE \r\n\r\n", strlen(BYE_MESSAGE)); /*send bye*/
        Close(connfd);/*close connection*/
        pthread_exit(NULL);
        return NULL;

      }

    } else {
      Close(connfd);/*he aint from around, we get him out*/

    }

  } else {
    Close(connfd);/*he aint from around, we get him out*/
  }



  // printf("%s\n", "EXITING THREAD");
  if (number_of_users > 0 && (comm_created == 0)) {
    pthread_t tid;
    // int *p = Malloc(sizeof(int));
    // *p = connfd;
    comm_created = 1;
    Pthread_create(&tid, NULL, comm_th, NULL);
    // printf("%s\n", "yes");
  }
  pthread_exit(NULL);
  return NULL;
}


void sha256(char string[MAXLINE], char outputBuffer[65]) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, string, strlen(string));
  SHA256_Final(hash, &sha256);
  int i = 0;
  for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
  }
  outputBuffer[64] = 0;
}

// void echo(int connfd) {
//   size_t n;
//   char buf[MAXLINE];
//   // rio_t rio;

//   // Rio_readinitb(&rio, connfd);
//   while ((n = read(connfd, buf, MAXLINE)) != 0 ) {
//     printf("Server received %d bytes received %s\n", (int)n, buf);
//     // Rio_writen(connfd, buf,  n);
//   }
// }

/*Shutdown's the Server*/
void serverShutdown() {
  if (head != NULL) {
    Users *cursor = head;
    for (; cursor != NULL; cursor = cursor -> next_user) {
      int fd = cursor -> socketfd;
      write(fd, BYE_MESSAGE, strlen(BYE_MESSAGE) + 1); /*sends bye to user's fd*/
      close(fd);
      // Free(cursor);/*free structs*/
    }
  }
  exit(0);
}
/******************************/

/*Prints the help menu*/
void printHelp() {
  fprintf(stderr,                                                                                                     \
          "\n\x1B[1;37mSi-coders CSE320 Server, version 4.3.42(1)-release (x86_64-pc-linux-gnu) \n"                                                                           \
          "\n"                                                                                                            \
          "\x1B[1;37mThe list of the commands this server supports:\n"                                        \
          "\n"                                                                                                            \
          "\x1B[1;37m/users            Prints out a nicely formatted listed of the users currently logged-in.\n"                                                                                         \
          "\x1B[1;37m/help             Prints this help menu.\n"                                                    \
          "\x1B[1;37m/shutdown         Cleanly disconnects all connected users, closes all file descriptors and free's all used memory.\x1B[0m\n"                                                                                                                                                                                                             \
         );
}
/******************************/

/*Prints out the list of users*/
void printUsers(void) {
  fprintf(stderr,"%s\n", "\x1B[1;31mDisplaying Logged in Users: ....\x1B[0m");
  if (head != NULL) {
    Users *cursor = head;
    for (; cursor != NULL; cursor = cursor -> next_user) {
      fprintf(stderr,"\x1B[1;37mUsername: \x1B[1;36m%s\x1B[0m\n", cursor -> name);
      fprintf(stderr,"\x1B[1;37mFile Descriptor: \x1B[1;36m%d\x1B[0m\n", cursor -> socketfd);
      fprintf(stderr,"\x1B[1;37mIP: \x1B[1;36m%d\x1B[0m\n", cursor->ip);
      fprintf(stderr,"\x1B[1;37mTime Connected: \x1B[1;36m%lu\x1B[0m\n\n", cursor -> timeConnected);
    }
  } else {
    fprintf(stderr,"%s\n", "\x1B[1;37mThe list is empty!\x1B[0m");
  }
}
void print_accounts(void) {
  fprintf(stderr,"%s\n", "\x1B[1;31mDisplaying Accounts on Server: ....");
  if (accounts_head != NULL) {
    Accts *cursor = accounts_head;
    for (; cursor != NULL; cursor = cursor -> next_acct) {
      fprintf(stderr,"\x1B[1;37mUsername: \x1B[1;36m%s\x1B[0m\n", cursor -> name);
      // printf("\x1B[1;37mPassword: \x1B[1;36m%s\x1B[0m\n", cursor -> );
    }
  } else {
    fprintf(stderr,"%s\n", "\x1B[1;37mThe list is empty!\x1B[0m");
  }
}
/******************************/



// void read_aim(int connfd){
//   size_t n;
//   char buf[MAXLINE];
//   // rio_t rio;

//   // Rio_readinitb(&rio, connfd);
//   while((n = read(connfd, buf, 1)) != 0  ){
//     printf("Server received %d bytes received %s\n", (int)n, buf);
//     // Rio_writen(connfd, buf,  n);
//   }
// }


/*sio reverse*/
/*reverses a string */
// char *strrev(char *str) {
//   char temp;
//   int i = 0;
//   int j = strlen(str) - 1;

//   while (i < j) {
//     temp = str[i];
//     str[i] = str[j];
//     str[j] = temp;
//     i++;
//     j--;
//   }
//   return str;
// }
/*BACKUP NO PART 3*/
/*********************************Login_thread**************************************/

void *login_thread(void *vargp) {

  int connfd = *((int*)vargp);
  printf("%d\n", connfd);
  Pthread_detach(pthread_self());
  Free(vargp);
  char message[MAXLINE];
  char *iam[3];
  read(connfd, message, strlen(WOLFIE) + 1);

  if (strcmp(message, WOLFIE) == 0) {
    printf("%s\n", "yes");
    write(connfd, WOLFIE_BACKWARDS, strlen(WOLFIE_BACKWARDS) + 1);
    memset(message, 0, strlen(message) + 1);

    read(connfd, message, MAXLINE);/*read next message*/
    printf("MESSAGE GOT %s\n", message);
    tokenize(message, iam);


    if (strcmp(iam[0], IAM_MESSAGE) == 0) {
      memset(message, 0, strlen(message) + 1); /*clear mem*/
      Users *test = Malloc(sizeof(struct Users));
      strcpy(test->name, iam[1]);
      test -> socketfd = connfd;
      test -> ip = 0;
      test -> timeConnected = time(NULL);
      if (addUser(&head, test)) {

        number_of_users++;
        addevent(connfd);/*add event*/

      } else {
        write(connfd, ERR_MESSAGE, strlen(ERR_MESSAGE) + 1); /*send err*/
        write(connfd, BYE_MESSAGE, strlen(BYE_MESSAGE) + 1); /*send bye*/

        memset(message, 0, strlen(message) + 1);

        read(connfd, message, MAXLINE);/*read next message*/
        if (strcmp(message, BYE_MESSAGE) == 0) {
          Close(connfd);/*close connection*/
          pthread_exit(NULL);
          return NULL;
        } else { /*else we still close*/
          Close(connfd);/*close connection*/
          pthread_exit(NULL);
          return NULL;
        }
        printf("%s\n", "repeated user or some other thing that we ran into");
        printf("didn't add the user properly\n");
      }

      reply_message(HI_MESSAGE, message, iam[1]);/*create message*/


      write(connfd, message, strlen(message) + 1);

      /*Copy the message*/
      memset(message, 0, strlen(message) + 1); /*clear mem*/
      // strcpy(message,argv[2]);
      strcpy(message, "TEST");
      write(connfd, message, strlen(message) + 1);
      /*make sure its the right argv*/
    } else {
      Close(connfd);/*he aint from around, we get him out*/

    }

  } else {
    Close(connfd);/*he aint from around, we get him out*/
  }

  printf("%s\n", "exiting");
  // pause();

  if (number_of_users > 0 && (comm_created == 0)) {
    pthread_t tid;
    // int *p = Malloc(sizeof(int));
    // *p = connfd;
    comm_created = 1;
    Pthread_create(&tid, NULL, comm_th, NULL);
    printf("%s\n", "yes");
  }
  pthread_exit(NULL);
  return NULL;
}
/*********************************Comm Thread**************************************/
int efd;
void *comm_th(void *vargp) {
  while (1 && (number_of_users > 0)) {
    int n, i;
    n = epoll_wait(efd, events, MAXLINE, -1);
    for (i = 0; i < n; i++) {
      if (events[i].events & EPOLLERR) {
        printf("%s\n", "Communication error.");
        exit(1);
      } else if ((events[i].events & EPOLLIN)) {
        read_message(events[i].data.fd, i);
      }
    }
  }
  comm_created = 0;
  pthread_exit(NULL);
  return NULL;
}

/*********************************Addevent**************************************/
void addevent(int fd) {
  // printf("ADDED %d\n", fd);
  static int created = 0, event_number = 1;
  struct epoll_event event;
  if ((created == 0) || (event_number == 1)) {
    created++;
    efd = epoll_create(MAXLINE - 1);
  }
  event.data.fd = fd;
  // printf("FD %d\n", fd);
  event.events = EPOLLIN;
  int s = epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event);
  if (s == -1)
    printf("%s\n", "error");
  events[event_number++] = event;
  // printf("Events %d\n", event.data.fd);
  // int j;
  // for (j = 0; j < 4; j++ ) {
  //   printf("INSIDE HI EVENTTTTTT %d\n", events[j].data.fd);
  // }
}


/************************Sig Handlers*******************************/


void ctrl_c_handler(int signo) {
  if (signo == SIGINT)
    serverShutdown();
}

void read_message(int fd, int index) {
  // printf("%s\n", "inside read_message");
  // int j = 0;
  // // printf("%s\n", "First inside Read_Messsage:");
  // for (j = 0; j < 2; j++ ) {
  //   printf("%d\n", events[j].data.fd);
  // }
  int k = 1;
  for (; k < MAXLINE; k++ ) {
    if (events[k].data.fd == fd) {
      break;
    }
  }

  // printf("K %d\n", k);
  char last_char;
  char *cursor;
  char buf[MAXLINE];
  memset(buf, 0, MAXLINE);
  // int rv;
  cursor = buf;
  while (strstr(buf, "\r\n\r\n") == NULL) {
    read(fd, &last_char, 1);
    // printf("%s char got %c \n", "inside while", last_char);
    *cursor = last_char;
    cursor++;
  }
  *cursor = '\0';
  if(verbose == 1)
    fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", buf);
  // printf("BUFFER %s pop %d\n", buf, rv);
  if (strstr(buf, "HI") != NULL) {
    // printf("%s\n", "got hi");
    // int j;
    // for (j = 0; j < 4; j++ ) {
    //   printf("INSIDE HI EVENTTTTTT %d\n", events[j].data.fd);
    // }
    write(fd, "HI\r\n\r\n", strlen("HI\r\n\r\n"));
  } else if (strstr(buf, "MSG") != NULL) {
    char msg[MAXLINE];
    strcpy(msg, buf);
    char *tokens[MAXLINE];
    tokenize(buf, tokens);
    int to_fd;
    if ((to_fd = get_user_fd(&head, tokens[1])) > 0) {
      // printf("msg %s\n", msg);
      // printf("fd %d to_fd %d\n", fd, to_fd);
      write(fd, msg, strlen(msg));
      write(to_fd, msg, strlen(msg));
    } else {
      /*send the err message that the user is not connected*/
      write(fd, ERR_MESSAGE_USER_NOT_AVAILABLE, strlen(ERR_MESSAGE_USER_NOT_AVAILABLE));
    }

    // pause();
  } else if ((strstr(buf, "TIME") != NULL)) {
    char message[MAXLINE];
    time_t diff = getEndTime(getUserByFd(&head, fd)->timeConnected);
    // printf("%s\n", "Received /time from user");
    char str[MAXLINE];
    sprintf(str, "%lu", diff);
    // printf("The seconds in a string: %s\n", str);
    reply_message(TIME_MESSAGE, message, str);
    write(fd, message, strlen(message));
  } else if ((strstr(buf, "BYE") != NULL)) {
    int fdToRemove = events[k].data.fd;
    // printf("%s\n", "inside bye");
    // printf("GUY WE WANT TO REMOVE %d index %d\n", events[k].data.fd, k);
    // printf("%s\n", "Before removal:");
    // for (i = 0; i < 5; i++ ) {
    //   printf("%d\n", events[i].data.fd);
    // }
    char nam[MAXLINE];
    strcpy(nam, getUserByFd(&head, fdToRemove)->name);
    epoll_ctl(efd, EPOLL_CTL_DEL, fd, &events[k]);
    removeFromArray(events, k);
    removeUser(&head, getUserByFd(&head, fdToRemove)->name);
    number_of_users--;
    /*send message to other CLients*/
    char temp[MAXLINE];
    reply_message(UOFF, temp, nam);
    Users *cursor = head;
    for (; cursor != NULL; cursor = cursor -> next_user) {
      // printf("MESSAGE: %s TO: %d\n", temp, cursor-> socketfd);
      write(cursor-> socketfd, temp, strlen(temp));
    }

    // printf("%s\n", "After removal:");
    // for (i = 0; i < 5; i++ ) {
    //   printf("%d\n", events[i].data.fd);
    // }
    // pause();
  } else if ((strstr(buf, "LISTU") != NULL)) {
    char msg[MAXLINE];
    // create_listu(buf, msg);
    strcpy(msg, "UTSIL ");
    Users *cursor = head;
    for (; cursor != NULL; cursor = cursor -> next_user) {
      strcat(msg, cursor->name);/*TO*/
      strcat(msg, " ");/**/
      strcat(msg, "\r\n");/**/
      strcat(msg, " ");/**/
    }
    strcat(msg, END_STRING);/*ADD the end*/
    write(fd, msg, strlen(msg));
  }
  /*users*/
  /*msg*/

}


void write_accounts() {
  char string[MAX_INPUT];
  strcpy(string, getenv("HOME"));
  char *fileName = strcat(string, "/.Server_Accounts");
  // printf("Writing to: %s\n", fileName);
  unlink(fileName);
  //char *fileName = "testfile";
  FILE *file = fopen(fileName, "w+");
  if (head != NULL) {
    Accts* cursor = accounts_head;
    for (; cursor != NULL; cursor = cursor -> next_acct) {
      fputs(cursor->name, file);
      fputs(" ", file);
      fputs(cursor->saltValue, file);
      fputs(" ", file);
      fputs(cursor->hashedPass, file);
      fputs("\n", file);
    }
  }
  fclose(file);
}

void read_accounts() {
  char string[MAX_INPUT];
  char *tokens[MAXLINE];
  strcpy(string, getenv("HOME"));
  char *fileName = strcat(string, "/.Server_Accounts");
  // printf("Reading from: %s\n", fileName);
  //char *fileName = "testfile";
  FILE* file = fopen(fileName, "r");
  char input[MAX_INPUT];
  struct stat st;
  // printf("size %ld\n", st.st_size);
  if (stat(fileName, &st) == 0) {
    while (fgets(input, MAX_INPUT, (FILE *)file)) {
      size_t length = strlen(input);
      if (input[length - 1] == '\n') {
        input[--length] = '\0';
      }
      tokenize(input, tokens);
      //printf("%s\n", tokens[1]);
      Accts *temp = Malloc(sizeof(struct Accts));
      strcpy(temp->name, tokens[0]);
      //printf("Read in: %s\n", temp->name);
      strcpy(temp->saltValue, tokens[1]);
      //printf("Read in salt: %s\n", temp->saltValue);
      strcpy(temp->hashedPass, tokens[2]);
      addAcct(&accounts_head, temp);
    }

    fclose(file);
  } else {
    fprintf(stderr, "%s\n", "file not found");
  }
}
