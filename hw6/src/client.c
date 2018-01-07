#include "hw5.h"
/*Messages to check when init connection*/

Chats *head = NULL;
int efd;
struct epoll_event events[MAXLINE];
int create = 0;
int verbose = 0;
int clientfd;/*descriptor to listen on*/
char audit_file_name[MAXLINE];
char name[MAXLINE];
char *host, *port, buf[MAXLINE];/*will store input*/
pthread_mutex_t count_mutex;

int main(int argc, char **argv) {
	char message[MAXLINE];
	int a = 0;
	strcpy(audit_file_name, "audit");

	if (argc < 3 || argc > 8) {
		fprintf(stderr, "%s\n", "usage: ./client [-hcv] [-a FILE] NAME SERVER_IP SERVER_PORT");
		exit(0);
	}
	int opt;
	while ((opt = getopt(argc, argv, "hcva:")) != -1) {
		switch (opt) {
		case 'v':
			// fprintf(stderr, "%s\n", "verbose");
			verbose = 1;
			break;
		case 'h':
			// fprintf(stderr, "%s\n", "help menu");
			printHelpClient();
			exit(0);
			break;
		case 'c':
			// fprintf(stderr, "%s\n", "c command");
			create = 1;
			// printf("%s\n", "create user");
			break;
		case 'a':
			a = 1;
			memset(audit_file_name, 0, MAXLINE);
			strcpy(audit_file_name, optarg);
			break;
		default:
			printf("%s\n", "got here");
			exit(0);
			break;
		}

	}

	if (argc == 4) {
		if ((create == 1) || (verbose == 1) || (a == 1)) {
			fprintf(stderr, "%s\n", "usage: ./client [-hcv] [-a FILE] NAME SERVER_IP SERVER_PORT");
			exit(0);
		} else {
			strcpy(name, argv[1]);
			host = argv[2];
			port = argv[3];
		}
	} else if (argc == 5) {
		if ((create == 1) && (verbose == 1) && (a == 1)) {
			fprintf(stderr, "%s\n", "usage: ./client [-hcv] [-a FILE] NAME SERVER_IP SERVER_PORT");
			exit(0);
		} else if (a == 1) {
			fprintf(stderr, "%s\n", "usage: ./client [-hcv] [-a FILE] NAME SERVER_IP SERVER_PORT");
			exit(0);
		} else if ((create == 1) || (verbose == 1)) {
			strcpy(name, argv[2]);
			host = argv[3];
			port = argv[4];
		}
	} else if (argc == 6) {
		if ((create == 1) && (verbose == 1) && (a == 1)) {
			fprintf(stderr, "%s\n", "usage: ./client [-hcv] [-a FILE] NAME SERVER_IP SERVER_PORT");
			exit(0);
		} else if ((create == 1) && (verbose == 1)) {
			strcpy(name, argv[3]);
			host = argv[4];
			port = argv[5];
		} else if (((create == 1) && (a == 1)) || ((verbose == 1) && (a == 1))) {
			fprintf(stderr, "%s\n", "usage: ./client [-hcv] [-a FILE] NAME SERVER_IP SERVER_PORT");
			exit(0);
		} else if ((a == 1) && (verbose == 0) && (create == 0)) {
			// memset(audit_file_name, 0, MAXLINE);
			//       strcpy(audit_file_name, argv[2]);
			strcpy(name, argv[3]);
			host = argv[4];
			port = argv[5];
		}
	} else if (argc == 7) {
		if ((create == 1) && (verbose == 1)) {
			fprintf(stderr, "%s\n", "usage: ./client [-hcv] [-a FILE] NAME SERVER_IP SERVER_PORT");
			exit(0);
		} else {
			// memset(audit_file_name, 0, MAXLINE);
			// strcpy(audit_file_name, argv[3]);
			strcpy(name, argv[4]);
			host = argv[5];
			port = argv[6];
		}
	} else if (argc == 8) {
		// printf("%s\n", "inside here");
		// memset(audit_file_name, 0, MAXLINE);
		// strcpy(audit_file_name, argv[4]);
		strcpy(name, argv[5]);
		host = argv[6];
		port = argv[7];
	}

	// fclose(file);
	// printf("%d\n", verbose);
	/*Store value appropiately*/
	// if (argc == 4) {
	// 	strcpy(name, argv[1]);
	// 	host = argv[2];
	// 	port = argv[3];
	// } else if (argc == 5) {
	// 	strcpy(name, argv[2]);
	// 	host = argv[3];
	// 	port = argv[4];
	// } else if (argc == 6) {
	// 	strcpy(name, argv[3]);
	// 	host = argv[4];
	// 	port = argv[5];
	// } else if (argc == 7) {
	// 	strcpy(name, argv[4]);
	// 	host = argv[5];
	// 	port = argv[6];

	// }

	clientfd = Open_clientfd(host, port);

	// /*initiate connection*/
	// read_protocol(clientfd, name,  create, verbose);

	init_connexion(clientfd , name, verbose, create);

	struct epoll_event event;
	efd = epoll_create(MAXLINE - 1);

	event.data.fd = STDIN_FILENO;
	event.events = EPOLLIN;

// int s = epoll_ctl(efd, EPOLL_CTL_ADD, clientfd, &event);
	int s = epoll_ctl(efd, EPOLL_CTL_ADD, STDIN_FILENO, &event);

	if (s == -1) {
		printf("%s\n", "error");
	}

	struct epoll_event event_clientfd;
	event_clientfd.data.fd = clientfd;
	event_clientfd.events = EPOLLIN | EPOLLET;


	s = epoll_ctl(efd, EPOLL_CTL_ADD, clientfd, &event_clientfd);

	if (s == -1) {
		printf("%s\n", "error");
	}

	events[1] = event;
	events[2] = event_clientfd;
	// printf("Client %d\n", clientfd);
	// static const int parentsocket = 0;
	// static const int childsocket = 1;
	// int j = 2;
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGCHLD, &sa, 0) == -1) {
		perror(0);
		exit(1);
	}

	if (Signal(SIGCHLD, children_handler) == SIG_ERR)/*handle the dead kids*/
		printf("\ncan't catch SIGINT\n");
	if (Signal(SIGINT, ctrl_c_handler_client) == SIG_ERR)/*handle ctrl-c*/
		printf("\ncan't catch SIGINT\n");
	while (1) {
		int n, i;

		n = epoll_wait(efd, events, MAXLINE, -1);
		for (i = 0; i < n; i++) {
			// printf("i %d\n", i);
			if (events[i].events & EPOLLERR) {
				printf("%s\n", "Connection error.");
				exit(1);

			} else if ((clientfd == events[i].data.fd) &&  (events[i].events & EPOLLIN)) {
				/*check if chat exists*/
				// printf("%s\n", "getting stuff");
				send_message(clientfd, name);

			} else if ((STDIN_FILENO == events[i].data.fd) &&  (events[i].events & EPOLLIN)) {
				// printf("%s\n", "inside stdin of client");
				// pause();
				if (!Fgets(buf, MAXLINE, stdin))
					exit(0);
				if (strstr(buf, "/users") != NULL) {
					// printf("%s\n", "display users: ....");
					write(clientfd, "HI\r\n\r\n", strlen("HI\r\n\r\n")); /*send Wolfie*/
					read(clientfd, message, MAXLINE);
					// printf("RECEIVED %s\n", message);
				} else if (strstr(buf, "/time") != NULL) { /*Ask for time logged in*/
					// printf("%s\n", "Sending /time to server");
					FILE* file = fopen(audit_file_name, "a");
					sfwrite(&count_mutex, file, "%s, %s, %s, %s, success, %s", getDateTimeStamp(), name, "CMD", "/time", "client");
					Fclose(file);
					write(clientfd, TIME_MESSAGE_SEND, strlen(TIME_MESSAGE_SEND));
				} else if (strstr(buf, "/logout") != NULL) {
					FILE* file = fopen(audit_file_name, "a");
					sfwrite(&count_mutex, file, "%s, %s, %s,%s", getDateTimeStamp(), name, "LOGOUT", "intentional");
					Fclose(file);
					client_shutdown();
				} else if (strstr(buf, "/listu") != NULL) {
					FILE* file = fopen(audit_file_name, "a");
					sfwrite(&count_mutex, file, "%s, %s, %s, %s, success, %s", getDateTimeStamp(), name, "CMD", "/listu", "client");
					Fclose(file);
					write(clientfd, LISTU, strlen(LISTU));
				} else if (strstr(buf, "/help") != NULL) {
					FILE* file = fopen(audit_file_name, "a");
					sfwrite(&count_mutex, file, "%s, %s, %s, %s, success, %s", getDateTimeStamp(), name, "CMD", "/help", "client");
					Fclose(file);
					printHelpClient();
				} else if (strstr(buf, "/chat") != NULL) {/*contains chat*/
					char msg[MAXLINE];
					memset(msg, 0, MAXLINE);/*clear it*/
					char buf2[MAXLINE];
					char msg2[MAXLINE];
					memset(msg2, 0, MAXLINE);/*clear it*/
					strcpy(buf2, buf);
					int err = create_msg(buf, msg, name);
					//FILE* file = fopen(audit_file_name, "a");
					int writeToFile = writeMsgToFile(buf2, msg2, name, audit_file_name, count_mutex);
					// printf("%s\n", msg);

					//printf("%s\n", "got here");
					if (writeToFile) {
						fprintf(stderr, "%s\n", "Error writing to audit file.");
					}
					if (!err)
						write(clientfd, msg, strlen(msg));
					else
						fprintf(stderr, "%s\n", "Format: /chat <TO> <MSG>");

					// 	pid_t pid;

					// 	/* 1. call socketpair ... */
					// 	socketpair(AF_UNIX, SOCK_STREAM, 0, chats[j]);
					// 	/* 2. call fork ... */
					// 	if ((pid = fork()) == 0) { /* 2.1 if fork returned zero, you are the child */
					// 		close(chats[j][parentsocket]); /* Close the parent file descriptor */
					// 		launch_chat(chats[j][childsocket], name, 10);/*replace with name "test"*/

					// 	} else { /* 2.2 ... you are the parent */
					// 		close(chats[j][childsocket]); /* Close the child file descriptor */
					// 		struct epoll_event ev;/*struct where to store the info*/
					// 		ev.data.fd = chats[j][parentsocket];/*file descriptor to add*/
					// 		ev.events = EPOLLIN;/*it reads something*/
					// 		s = epoll_ctl(efd, EPOLL_CTL_ADD, chats[j][parentsocket], &ev);/*add to waiting events*/
					// 		events[j++] = ev;/*add to array of events*/
					// 	}
				} else if (strstr(buf, "/audit") != NULL) {
					// printf("%s\n", audit_file_name);
					read_audit(audit_file_name);
				} else {
					if(!strcmp(buf, "\n")){
						strcpy(buf, "newline\n");
					}
					FILE* file = fopen(audit_file_name, "a");
					sfwrite(&count_mutex, file, "%s, %s, %s, %s, failure, %s", getDateTimeStamp(), name, "CMD", strtok(buf, "\n"), "client");
					Fclose(file);
					printf("%s\n", "Not a valid command, type /help for more.");
				}/*end chat*/
			} else if ((events[i].events & EPOLLIN)) {
				// printf("%s\n", "a kid");
				read_message_from_chat(events[i].data.fd, name);
				// pause();
				// continue;
			}


		}
	}/*end while*/

	Close(clientfd);
	return 0;
}

void read_message_from_chat(int fd, char *name) {
	char last_char = ' ';
	char *cursor;
	char buf[MAXLINE];
	// char msg[MAXLINE];
	memset(buf, 0, MAXLINE);
	// memset(msg, 0, MAXLINE);

	int rv;
	cursor = buf;
	while (last_char != '\0') {
		// printf("%s\n", "INSI/DE WHILE READ MESSAGE FROM CHAT");
		// printf("%c\n", last_char);
		rv = read(fd, &last_char, 1);
		if (rv == 0 || rv == -1) {
			// printf("%s\n", "ERRRROROROOROROROROROOR");
			kill_chat(fd, 0);
			// printf("%s\n", "came back");
			goto done;
		}
		// printf("%s char got %c \n", "inside while", last_char);
		*cursor = last_char;
		cursor++;
	}
	*cursor = '\0';
	Chats* test = getChatByFd(&head, fd);
	if (strstr(buf, "/close") != NULL) {
		FILE* file = fopen(audit_file_name, "a");
		sfwrite(&count_mutex, file, "%s, %s, %s, %s, success, %s", getDateTimeStamp(), name, "CMD", "/close", "chat");
		Fclose(file);
		kill_chat(fd, 1);
		// int k = 1;
		// for (; k < MAXLINE; k++ ) {
		// 	if (events[k].data.fd == fd) {
		// 		break;
		// 	}
		// }
		// epoll_ctl(efd, EPOLL_CTL_DEL, fd, &events[k]);
		// removeFromArray(events, k);
		// Kill(test->pid, SIGKILL);
		// removeChat(&head, test->name);

	} else {

		char msg[MAXLINE];
		// printf("What it got from chat: %s %d name to %s from %s\n", buf, rv, test->name, name);
		// char *tokens[MAXLINE];

		// tokens[1] = test->name;
		strcpy(msg, "MSG ");
		strcat(msg, test->name);
		strcat(msg, " ");
		strcat(msg, name);
		strcat(msg, " ");
		strcat(msg, buf);
		strcat(msg, " ");
		strcat(msg, END_STRING);

		// printf("FULL MESSAGE: %s\n", msg);

		write(clientfd, msg, strlen(msg));
		// pause();
	}
done:
	return;

}

int chats[MAXLINE][2];/*will store all file descriptors*/
void send_message(int fd, char *name) {
	// printf("%s\n", "inside read_message");
	// int forked;
	char last_char;
	char *cursor;
	char buf[MAXLINE];
	char msg[MAXLINE];
	memset(buf, 0, MAXLINE);
	memset(msg, 0, MAXLINE);
	static const int parentsocket = 0;
	static const int childsocket = 1;
	static int j = 3;
	int rv;
	cursor = buf;
	while (strstr(buf, "\r\n\r\n") == NULL) {
		// printf("%s\n", "IINSIDE WHILE SEND MESSAGE");
		rv = read(fd, &last_char, 1);
		// printf("%s char got %c \n", "inside while", last_char);
		if (rv == 0 || rv == -1) {
			kill_chat(fd, 0);
			// printf("%s\n", "came back");
		}
		*cursor = last_char;
		cursor++;
	}
	*cursor = '\0';
	if (verbose == 1)
		fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", buf);
	// printf("BUFFER %s pop %d\n", buf, rv);
	if (strstr(buf, "HI") != NULL) {
		// printf("%s\n", "done");
	} else if (strstr(buf, "EMIT") != NULL) {
		// printf("Received time backwards from server: %s\n", buf);
		char *tokens[MAXLINE];
		tokenize(buf, tokens);
		char *ptr;
		long s = strtol(tokens[1], &ptr, 10);
		int hours = s / 3600;
		int leftOver = s - (hours * 3600);
		int minutes = leftOver / 60;
		leftOver -= minutes * 60;
		int seconds = leftOver;
		char timePrint[80];
		sprintf(timePrint, "connected for %d hour(s), %d minutes(s), and %d second(s)\r\n\r\n", hours, minutes, seconds);
		printf("\x1B[1;37m%s\x1B[0m\n", timePrint);
	} else if (strstr(buf, "MSG") != NULL) {
		strcpy(msg, buf);/*save original buf*/
		char first_message[MAXLINE];
		char *tokens[MAXLINE];
		int size = tokenize(buf, tokens);
		char *n, symbol[2] = {'\0', '\0'}; /*name to use*/
		if (!strcmp(name, tokens[1])) {/*name equal to to*/
			n = tokens[2];/*set the from being the name*/
			symbol[0] = '>';
		}
		else if (!strcmp(name, tokens[2])) {/*if name equal to from, set to to name*/
			n = tokens[1];
			symbol[0] = '<';
		}

		if (!checkChat(&head, n)) {
			/*check if its not there already*/
			/*add chat stuff to struct*/
			pid_t pid;

			/* 1. call socketpair ... */
			socketpair(AF_UNIX, SOCK_STREAM, 0, chats[j]);
			/* 2. call fork ... */
			if ((pid = fork()) == 0) { /* 2.1 if fork returned zero, you are the child */
				close(chats[j][parentsocket]); /* Close the parent file descriptor */
				// forked = 1;
				launch_chat(chats[j][childsocket], n, 10);/*replace with name "test"*/
				printf("%s\n", "it did return");
				_exit(1);
			} else { /* 2.2 ... you are the parent */
				/*add user*/
				Chats *test = Malloc(sizeof(struct Chats));
				strcpy(test->name, n);/*to who*/
				test->socketfd = chats[j][parentsocket];
				test->pid = pid;
				addChat(&head, test);/*adds the chat*/

				// printf("%s\n", "else");
				close(chats[j][childsocket]); /* Close the child file descriptor */
				strcpy(first_message, symbol);
				strcat(first_message, " ");/*space after symbol*/
				// printf("%s\n", symbol);
				int o = 3;
				while (size - 1 > o) {
					strcat(first_message, tokens[o++]);
					strcat(first_message, " ");
				}
				// strcat(first_message, END_STRING);/*ADD the end*/
				// printf("First message: %s\n", first_message);
				write(chats[j][0], first_message, strlen(first_message) + 1);
				struct epoll_event ev;/*struct where to store the info*/
				ev.data.fd = chats[j][parentsocket];/*file descriptor to add*/
				ev.events = EPOLLIN;/*it reads something*/
				int s = epoll_ctl(efd, EPOLL_CTL_ADD, chats[j][parentsocket], &ev);/*add to waiting events*/
				if (s == -1) {
					printf("%s\n", "error");
				}
				events[j++] = ev;/*add to array of events*/
			}
		} else { /*end of checkchat*/
			/*else the chat is already up so we just display the msg*/
			// printf("TOKEN LAST%s\n", tokens[size - 1]);
			// printf("%s\n", "chat already up");
			strcpy(first_message, symbol);
			strcat(first_message, " ");/*space after symbol*/
			// printf("%s\n", symbol);
			int o = 3;
			while (size - 1 > o) {
				strcat(first_message, tokens[o++]);
				strcat(first_message, " ");
			}

			char messageDirection[MAXLINE];
			if (!strcmp(symbol, "<")) {
				strcpy(messageDirection, "to");
			} else {
				strcpy(messageDirection, "from");
			}

			FILE* file = fopen(audit_file_name, "a");
			sfwrite(&count_mutex, file, "%s, %s, %s, %s, %s, %s", getDateTimeStamp(), name, "MSG", messageDirection, n, strstr(strtok(first_message, "\n"), " "));
			Fclose(file);
			// strcat(first_message, END_STRING);/*ADD the end*/
			// printf("First message: %s\n", first_message);
			write(get_chat_fd(&head, n), first_message, strlen(first_message) + 1);
		}
		/*else we fork*/
	} else if ((strstr(buf, "BYE") != NULL)) {
		Close(fd);
		exit(0);
	} else if ((strstr(buf, "UTSIL") != NULL)) {
		// printf("%s\n", "inside LISTU");
		print_users_client(buf);
	} else if ((strstr(buf, UOFF) != NULL)) {
		// printf("%s %s\n", "RECEIVED ", buf);
		char *tokens[4];
		tokenize(buf, tokens);

		if (checkChat(&head, tokens[1])) {
			// printf("TOKEN NAME: %s\n", tokens[1]);
			// Chats* temp = get_chat_fd(&head, name);
			write(get_chat_fd(&head, tokens[1]), "User not available ERR02.", strlen("User not available ERR02.") + 1);
		}
	} else if ((strstr(buf, "ERR 01") != NULL)) {
		fprintf(stderr, "%s\n", "User not available.");
	}

	// if (forked) {
	// 	printf("%s\n", "forkeeeeed");
	// }

	/*users*/
	/*msg*/

}
/*********************************Launch_chat**************************************/
/*Forks a chat for specified user*/
int launch_chat(int fd, char *name, int size) {
	time_t t;
	srand((unsigned) time(&t));
	// int r = rand() % 8;
	int ra = rand() % 7;
	char *colors[8] = {"blue", "green", "yellow", "cyan", "white", "magenta", "red", NULL};

	char *str[15];/*arguments for xterm*/
	char buf[MAXLINE];/*buffer to store temp array*/
	memset(buf, 0, MAXLINE);
	sprintf(buf, "xterm -fg %s -bg %s -geometry 45x35+%d -T %s -e ./chat %d", colors[0], colors[ra], size, name, fd);
	tokenize(buf, str);
	if (execvp("/usr/bin/xterm", str) == -1) {
		fprintf(stderr, "Error forking exiting%s\n", ".");
	}

	return 0;
}

/*********************************init_connexion**************************************/
// void init_connexion(int clientfd, char* name) {
// 	char message[MAXLINE];
// 	char *iam[3];
// 	// /*initiate connection*/
// 	write(clientfd, WOLFIE, strlen(WOLFIE) + 1); /*send Wolfie*/

// 	read(clientfd, message, strlen(WOLFIE) + 1); /*read what the client replies*/

// 	if (strcmp(message, WOLFIE_BACKWARDS) == 0) {
// 		memset(message, 0, strlen(message) + 1);
// 		reply_message(IAM_MESSAGE, message, name);/*create message*/
// 		write(clientfd, message, strlen(message) + 1); /*send Wolfie*/
// 		memset(message, 0, strlen(message) + 1);
// 		read(clientfd, message, MAXLINE);/*read what the client replies*/
// 		printf("MESSAGE READ %s\n", message);
// 		tokenize(message, iam);
// 		if (strcmp(iam[0], HI_MESSAGE) == 0) {
// 			printf("%s\n", "it is hi");
// 			/*read the welcome message*/
// 			memset(message, 0, strlen(message) + 1);
// 			read(clientfd, message, MAXLINE);/*read what the client replies*/
// 			printf("%s\n", message);
// 		}
// 		else if (strcmp(iam[0], "ERR") == 0) { /*for ERR*/
// 			memset(message, 0, strlen(message) + 1);
// 			read(clientfd, message, MAXLINE);
// 			if (strcmp(iam[0], BYE_MESSAGE) == 0) {
// 				write(clientfd, message, strlen(message) + 1); /*send bye back*/
// 				Close(clientfd);
// 				exit(1);
// 			}
// 			printf("MESSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSA %s\n", message);
// 		}
// 		else {
// 			/*error*/
// 			Close(clientfd);
// 			exit(1);
// 		}

// 	} else {/*else we close*/
// 		Close(clientfd);
// 		exit(1);
// 	}
// }
/*Prints the help menu*/
void printHelpClient() {
	fprintf(stderr,                                                                                                    	 \
	        "\n\x1B[1;37mSi-coders CSE320 Server, version 4.3.42(1)-release (x86_64-pc-linux-gnu) \n"                                                                           \
	        "\n"                                                                                                            \
	        "\x1B[1;37mThe list of the commands this server supports:\n"                                        				\
	        "\n"                                                                                                            \
	        "\x1B[1;37m/listu            Prints out a nicely formatted listed of the users currently logged-in.\n"                                                                                        \
	        "\x1B[1;37m/chat 				You can start a chat with someone.\n"                                                                                         \
	        "\x1B[1;37m/help             Prints this help menu.\n"                                                 		   \
	        "\x1B[1;37m/logout         Cleanly disconnects all connected users, closes all file descriptors and free's all used memory.\x1B[0m\n"                                                                                                                                                                                                             \
	       );
}
/************************Sig Handlers*******************************/
void children_handler(int sig) {
	int olderrno = errno;
	pid_t pid;
	// sigset_t mask_all, prev_all;
	// Sigfillset(&mask_all);
	while ((pid = waitpid(-2, NULL, 0) ) > 0 ) {

	}
	if (errno != ECHILD)
		Sio_error("waitpid error");
	// Sleep(1);
	errno = olderrno;
	// printf("%s\n", "leaving signla");
}


void ctrl_c_handler_client(int signo) {
	if (signo == SIGINT)
		client_shutdown();
}

void client_shutdown() {
	printf("%s\n", "shutting down");
	Write(clientfd, BYE_MESSAGE, strlen(BYE_MESSAGE) + 1); /*send Wolfie*/
	//Fclose(file);//close the file on this end
	FILE* file = fopen(audit_file_name, "a");
	sfwrite(&count_mutex, file, "%s, %s, %s,%s", getDateTimeStamp(), name, "LOGOUT", "error");
	Fclose(file);
	if (head != NULL) {
		Chats *cursor = head;
		for (; cursor != NULL; cursor = cursor -> next_chat) {
			int fd = cursor -> socketfd;
			kill_chat(fd, 1);
			// write(fd, BYE_MESSAGE, strlen(BYE_MESSAGE) + 1); /*sends bye to user's fd*/
			Close(fd);
			Free(cursor);/*free structs*/
		}
	}
	exit(1);
}



void kill_chat(int fd, int kill_it) {
	Chats* test = getChatByFd(&head, fd);
	int k = 1;
	for (; k < MAXLINE; k++ ) {
		if (events[k].data.fd == fd) {
			break;
		}
	}
	epoll_ctl(efd, EPOLL_CTL_DEL, fd, &events[k]);
	removeFromArray(events, k);
	if (kill_it == 1)
		kill(test->pid, SIGKILL);
	removeChat(&head, test->name);
	// printf("%s\n", "after removing");
}

void print_users_client(char *buf) {
	printf("%s\n", "\x1B[1;31mDisplaying Logged in Users: ....\x1B[0m");
	char *tokens[MAXLINE];

	int argc = tokenize(buf, tokens);
	int i = 1;
	for (; argc > i; i++) {
		fprintf(stderr, "%s\n", tokens[i]);
	}

	if (i == 1)
		fprintf(stderr, "%s\n", "There's no users.");
}






/*********************************init_connexion**************************************/
void init_connexion(int clientfd, char* name, int verbose, int create) {
	char message[MAXLINE];
	char *iam[3];
	// /*initiate connection*/
	if (verbose == 1)
		fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);

	write(clientfd, WOLFIE, strlen(WOLFIE)); /*send Wolfie*/

	// read(clientfd, message, strlen(WOLFIE)); /*read what the client replies*/
	memset(message, 0, MAXLINE);
	read_stuff(message, clientfd);
	if (verbose == 1)
		fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);
	if (strcmp(message, WOLFIE_BACKWARDS) == 0) {
		// printf("CREATE %d\n", create);
		if (create == 1) {

			memset(message, 0, strlen(message));
			reply_message(IAMNEW, message, name);/*create message*/
			write(clientfd, message, strlen(message)); /*send Wolfie*/

			memset(message, 0, strlen(message));
			// read(clientfd, message, MAXLINE);/*read what the client replies*/
			read_stuff(message, clientfd);
			if (verbose == 1)
				fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);

			tokenize(message, iam);
			if (strcmp(iam[0], HINEW) == 0) {
				/*read the welcome message*/
				char *password = Malloc(sizeof(char) * 30);
				password = getpass("Enter a password: ");
				/*Create message to send over*/
				memset(message, 0, strlen(message));
				reply_message(NEWPASS, message, password);/*create message*/

				// printf("SENT %s\n", message);
				write(clientfd, message, strlen(message)); /*send Wolfie*/

				/*check response according to password*/
				memset(message, 0, strlen(message));
				// read(clientfd, message, MAXLINE);/*read what the client replies*/

				// read_client(message, clientfd);/***?
				read_stuff(message, clientfd);


				if (verbose == 1)
					fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);
				if (strcmp(message, NEWPASS_BACKWARDS) == 0) {
					// printf("%s\n", "yesss");
					memset(message, 0, strlen(message) + 1);

					read_stuff(message, clientfd);
					if (verbose == 1)
						fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);
					// printf("%s\n", "after read");
					if (verbose == 1)
						fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);
					if (strstr(message, "HI") != NULL) {
						// printf("%s\n", "it does have hi");
						memset(message, 0, strlen(message) + 1);
						read_stuff(message, clientfd);

						if (verbose == 1)
							fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);
						// printf("%s %s\n", "BEFORE MOTD", message);

						if (strstr(message, "MOTD") != NULL) {
							char *tok[10];
							tokenize(message, tok);
							fprintf(stderr, "%s\n", tok[1]);
						} else {
							close(clientfd);
							exit(1);
						}

					} else {
						close(clientfd);
						exit(1);
					}
				} else if (strstr(message, "ERR") != NULL) {/*BADDDDD PASSSSWROODDDDD*/
					// printf("%s\n", "GOT TO BAD");
					FILE* file = fopen(audit_file_name, "a");
					//sfwrite(&count_mutex, file, "%s, %s, %s, fail, %s", getDateTimeStamp(), name, "LOGIN", message);
					sfwrite(&count_mutex, file, "%s, %s, %s, %s:%s, fail, %s", getDateTimeStamp(), name, "LOGIN", getIPAddress(clientfd), port, message);
					Fclose(file);

					read_stuff(message,clientfd);
					printf("%s\n",message);
					Close(clientfd);
					exit(1);
				} else {
					close(clientfd);
					exit(1);
				}

			} else if (strcmp(iam[0], "ERR") == 0) { /*for ERR*/

				FILE* file = fopen(audit_file_name, "a");
				sfwrite(&count_mutex, file, "%s, %s, %s, %s:%s, fail, %s", getDateTimeStamp(), name, "LOGIN", getIPAddress(clientfd), port, message);
				//sfwrite(&count_mutex, file, "%s, %s, %s, %s:%s, fail, ERR 02 BAD PASSWORD", getDateTimeStamp(), name, "LOGIN",getIPAddress(clientfd), port);
				Fclose(file);
				// memset(message, 0, strlen(message)+1);
				// printf("%s\n", "got and error");
				// read(clientfd, message, MAXLINE);
				// read_stuff(message, clientfd);
				// printf("%s\n", message);
				// pause();
				// if (strstr(message, BYE_MESSAGE) == 0) {
				// write(clientfd, message, strlen(message) + 1); /*send bye back*/
				Close(clientfd);
				exit(1);
				// }
			} else {
				/*error*/
				Close(clientfd);
				exit(1);
			}
		} else {/*ELSE  WE HAVE ACCOUNT ALREADY*/
			memset(message, 0, strlen(message) + 1);
			reply_message(IAM_MESSAGE, message, name);/*create message*/
			write(clientfd, message, strlen(message)); /*send Wolfie*/
			/*store password and stuff*/
			/**/
			memset(message, 0, strlen(message) + 1);
			// read(clientfd, message, MAXLINE);/*read what the client replies*/
			read_stuff(message, clientfd);
			if (verbose == 1)
				fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);
			// printf("MESSAGE READ %s\n", message);
			tokenize(message, iam);

			if (strcmp(iam[0], AUTH) == 0) {
				char *password = Malloc(sizeof(char) * 30);
				password = getpass("Enter a password: ");
				/*Create message to send over*/
				memset(message, 0, strlen(message));
				reply_message("PASS", message, password);/*create message*/
				Free(password);/*free password*/
				// printf("SENT pws %s\n", message);
				write(clientfd, message, strlen(message)); /*send message*/


				memset(message, 0, strlen(message));
				read_stuff(message, clientfd);
				// char last_char;
				// char *cursor;
				// int rv;
				// cursor = message;
				// while (strstr(message, "\r\n\r\n") == NULL) {
				// 	rv = read(clientfd, &last_char, 1);
				// 	if (rv == -1)
				// 		break;
				// 	*cursor = last_char;
				// 	cursor++;
				// }

				if (verbose == 1)
					fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);
				// pause();
				if (strcmp(message, PASS_BACKWARDS) == 0) {
					// printf("%s\n", "yesss");
					memset(message, 0, strlen(message) + 1);

					// /***********************************************/
					// /***********************************************/
					// pause();
					// /***********************************************/
					// /***********************************************/
					// char last_char;
					// char *cursor;
					// int rv;
					// cursor = message;
					// while (strstr(message, "\r\n\r\n") == NULL) {
					// 	rv = read(clientfd, &last_char, 1);
					// 	if (rv == -1)
					// 		break;
					// 	*cursor = last_char;
					// 	cursor++;
					// }
					read_stuff(message, clientfd);
					// printf("%s\n", "after read");
					if (verbose == 1)
						fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);
					if (strstr(message, "HI") != NULL) {
						// printf("%s\n", "it does have hi");
						memset(message, 0, strlen(message) + 1);
						read_stuff(message, clientfd);
						// char last_char;
						// char *cursor;
						// int rv;
						// cursor = message;
						// while (strstr(message, "\r\n\r\n") == NULL) {
						// 	rv = read(clientfd, &last_char, 1);
						// 	if (rv == -1)
						// 		break;
						// 	printf("%s\n", message);
						// 	*cursor = last_char;
						// 	cursor++;
						// }


						if (verbose == 1)
							fprintf(stderr, "\x1B[1;34m%s\x1B[0m\n", message);
						// printf("%s\n", "BEFORE MOTD");
						if (strstr(message, "MOTD") != NULL) {
							char *tok[10];
							tokenize(message, tok);
							FILE* file = fopen(audit_file_name, "a");
							sfwrite(&count_mutex, file, "%s, %s, %s, %s:%s, success, %s", getDateTimeStamp(), name, "LOGIN", getIPAddress(clientfd), port , tok[1]);
							Fclose(file);
							fprintf(stderr, "%s\n", tok[1]);
						} else {
							close(clientfd);
							exit(1);
						}

					} else {
						close(clientfd);
						exit(1);
					}
				} else if (strstr(message, "ERR") != NULL) {
					// printf("%s\n", "GOT TO BAD");
					FILE *file = fopen(audit_file_name, "a");
					sfwrite(&count_mutex, file, "%s, %s, %s, %s:%s, fail, ERR 02 BAD PASSWORD", getDateTimeStamp(), name, "LOGIN", getIPAddress(clientfd), port);
					Fclose(file);
					// memset(message, 0, strlen(message)+1);
					// printf("%s\n", "got and error");
					// read(clientfd, message, MAXLINE);
					// read_stuff(message, clientfd);
					// printf("%s\n", message);
					// pause();
					// if (strstr(message, BYE_MESSAGE) == 0) {
					// write(clientfd, message, strlen(message) + 1); /*send bye back*/
					Close(clientfd);
					exit(1);
				} else {
					close(clientfd);
					exit(1);
				}
			}
			else if (strcmp(iam[0], "ERR") == 0) { /*for ERR*/
				//printf("%s\n", "GOT TO BAD");
				FILE* file = fopen(audit_file_name, "a");
				sfwrite(&count_mutex, file, "%s, %s, %s, %s:%s, fail, ERR 02 BAD PASSWORD", getDateTimeStamp(), name, "LOGIN", getIPAddress(clientfd), port);
				Fclose(file);
				// memset(message, 0, strlen(message)+1);
				// printf("%s\n", "got and error");
				// read(clientfd, message, MAXLINE);
				// read_stuff(message, clientfd);
				// printf("%s\n", message);
				// pause();
				// if (strstr(message, BYE_MESSAGE) == 0) {
				// write(clientfd, message, strlen(message) + 1); /*send bye back*/
				Close(clientfd);
				exit(1);
			}
			else {
				/*error*/
				Close(clientfd);
				exit(1);
			}


		}

	} else {/*else we close*/
		Close(clientfd);
		exit(1);
	}
}



void read_audit(char *fileName) {
	FILE *file = fopen(audit_file_name, "r");
	//printf("Audit file name: %s\n", audit_file_name);
	int fileFD = fileno(file);
	//printf("File FD: %d\n", fileFD);
	char input[MAX_INPUT];
	struct stat st;
	// printf("size %ld\n", st.st_size);
	if (stat(audit_file_name, &st) == 0) {
		while (1) {
			Flock(fileFD, LOCK_SH);
			if (fgets(input, MAX_INPUT, file) != NULL) {
				fprintf(stderr, "%s", input);
				flock(fileFD, LOCK_UN);
			} else {
				//printf("%s\n", "inside read audit");
				flock(fileFD, LOCK_UN);
				break;
			}
		}
		Fclose(file);
	} else {
		fprintf(stderr, "%s\n", "file not found");
	}
}




/*********************************init_connexion**************************************/
// // void read_protocol(int fd, char *name);
// void init_connexion_password(int clientfd, char* name, char flag) {
// 	char message[MAXLINE];
// 	char *iam[3];
// 	// /*initiate connection*/
// 	write(clientfd, WOLFIE, strlen(WOLFIE) + 1); /*send Wolfie*/

// 	read(clientfd, message, strlen(WOLFIE) + 1); /*read what the client replies*/

// 	pause();
// 	if (strcmp(message, WOLFIE_BACKWARDS) == 0) {
// 		memset(message, 0, strlen(message) + 1);
// 		reply_message(IAM_MESSAGE, message, name);/*create message*/
// 		write(clientfd, message, strlen(message) + 1); /*send Wolfie*/
// 		memset(message, 0, strlen(message) + 1);
// 		read(clientfd, message, MAXLINE);/*read what the client replies*/
// 		printf("MESSAGE READ %s\n", message);
// 		tokenize(message, iam);
// 		if (strcmp(iam[0], HI_MESSAGE) == 0) {
// 			printf("%s\n", "it is hi");
// 			/*read the welcome message*/
// 			memset(message, 0, strlen(message) + 1);
// 			read(clientfd, message, MAXLINE);/*read what the client replies*/
// 			printf("%s\n", message);
// 		}
// 		else if (strcmp(iam[0], "ERR") == 0) { /*for ERR*/
// 			memset(message, 0, strlen(message) + 1);
// 			read(clientfd, message, MAXLINE);
// 			if (strcmp(iam[0], BYE_MESSAGE) == 0) {
// 				write(clientfd, message, strlen(message) + 1); /*send bye back*/
// 				Close(clientfd);
// 				exit(1);
// 			}
// 			printf("MESSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSA %s\n", message);
// 		}
// 		else {
// 			/*error*/
// 			Close(clientfd);
// 			exit(1);
// 		}

// 	} else {/*else we close*/
// 		Close(clientfd);
// 		exit(1);
// 	}
// }


// void read_protocol(int fd, char *name, int create, int verbose) {
// 	char last_char;
// 	char *cursor;
// 	char buf[MAXLINE];
// 	char msg[MAXLINE];
// 	int rv, done = 0;
// 	cursor = buf;
// 	write(clientfd, WOLFIE, strlen(WOLFIE)); /*send Wolfie*/
// 	if (verbose == 1)
// 		fprintf(stderr, "%s\n", WOLFIE);
// 	while (!done) {
// 		memset(msg, 0, MAXLINE);
// 		memset(buf, 0, MAXLINE);/*clear buff*/

// 		while (strstr(buf, "\r\n\r\n") == NULL) {
// 			rv = read(fd, &last_char, 1);
// 			if (rv == -1)
// 				break;
// 			printf("%s char got %c \n", "inside while", last_char);
// 			*cursor = last_char;
// 			cursor++;
// 		}
// 		if (strcmp(buf, WOLFIE_BACKWARDS) == 0 ) {
// 			if (verbose == 1)
// 				fprintf(stderr, "%s\n", buf);
// 			reply_message(IAMNEW, msg, name);/*create message*/
// 			printf("%s\n", msg);
// 			write(clientfd, msg, strlen(msg)); /*send Wolfie*/
// 			// pause();
// 			continue;
// 		} else if ((strstr(buf, HINEW) != NULL)) { /*got hi new*/
// 			/*promp for password*/
// 			char password[MAXLINE];
// 			reply_message(NEW_PASS, msg, password);/*create password msg*/
// 			write(clientfd, msg, strlen(msg)); /*send Wolfie*/
// 			/*NEW PASS*/
// 			continue;
// 		} else if (strstr(buf, "SSAPWEN") != NULL) {
// 			if (verbose == 1)
// 				fprintf(stderr, "%s\n", buf);
// 			continue;
// 		} else if (strstr(buf, "HI") != NULL) {
// 			if (verbose == 1)
// 				fprintf(stderr, "%s\n", buf);
// 			continue;
// 		} else if (strstr(buf, "MOTD") != NULL) {
// 			fprintf(stderr, "%s\n", buf);
// 			done = 1;
// 		} else {
// 			close(fd);
// 			exit(1);
// 		}
// 	}/*end while*/
// 	printf("%s\n", "EXITING LOGIN");
// }