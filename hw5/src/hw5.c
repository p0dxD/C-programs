#include "hw5.h"
/*Shared functions*/
void reply_message(char *message, char *dest, char *name) {
	strcpy(dest, message);
	strcat(dest, " ");
	strcat(dest, name);
	strcat(dest, " ");
	strcat(dest, END_STRING);
}

int create_msg(char *buf, char *msg, char *name) {
	char *tokens[MAXLINE];
	int tokenc = tokenize(buf, tokens);
	if (tokenc < 3) {
		return 1;
	}
	strcpy(msg, "MSG ");
	strcat(msg, tokens[1]);/*TO*/
	strcat(msg, " ");/**/
	strcat(msg, name);/*FROM*/
	strcat(msg, " ");/**/
	int i = 2;
	for (; i < tokenc; i++) {
		strcat(msg, tokens[i]);/*ADD the rest*/
		strcat(msg, " ");/**/
	}

	strcat(msg, END_STRING);/*ADD the end*/

	return 0;
}

// int create_listu(char *buf, char *msg) {
// 	char *tokens[MAXLINE];
// 	int tokenc = tokenize(buf, tokens);
// 	if(tokenc <3){
// 		return 1;
// 	}
// 	strcpy(msg, "MSG ");
// 	strcat(msg, tokens[1]);/*TO*/
// 	strcat(msg, " ");/**/
// 	strcat(msg, name);/*FROM*/
// 	strcat(msg, " ");/**/
// 	int i = 2;
// 	for (; i < tokenc; i++) {
// 		strcat(msg, tokens[i]);/*ADD the rest*/
// 		strcat(msg, " ");/**/
// 	}

// 	strcat(msg, END_STRING);/*ADD the end*/

// 	return 0;
// }

void removeFromArray(struct epoll_event *events, int index) {
	int n = MAXLINE;
	int c = 0;
	// int i = 0;
	// printf("INDEX FROM REMOVE ARRAY: %d\n", index);
	// printf("%s\n", "Inside remove array");
	// for (i = 0; i < 5; i++ ) {
	// 	printf("%d\n", events[i].data.fd);
	// }
	for (c = index; c < n - 1; c++) {
		events[c] = events[c + 1];
	}

	// printf("%s\n", "FTER remove array");
	// for (i = 0; i < 5; i++ ) {
	// 	printf("%d\n", events[i].data.fd);
	// }
}

time_t getEndTime(time_t beginTime) {
	time_t currentTime = time(NULL);
	return (currentTime - beginTime);
}

int addAcct(Accts **head, Accts *acct) {
	if (*head != NULL) {
		Accts *cursor = *head;
		if (!checkAcct(head, acct -> name)) {
			for (; cursor != NULL; cursor = cursor -> next_acct) {
				if (cursor -> next_acct == NULL) {
					cursor -> next_acct = acct;
					acct -> next_acct = NULL;
					return 1;
				}
			}
		}
	} else {
		acct -> next_acct = NULL;
		*head = acct;
		return 1;
	}
	return 0;
}

int removeAcct(Accts **head, char *acctToBeRemoved) {
	if (*head != NULL) {
		Accts *cursor = *head;
		if (checkAcct(head, acctToBeRemoved)) {	/*Must be in the list to remove it*/
			if (!strcmp(cursor->name, acctToBeRemoved)) {	/*If the user is at the head of the list*/
				// printf("Deleting from the head of the list\n");

				*head = cursor -> next_acct; /*Make the next user the new head*/
				return 1;
			} else {
				/*loop through and find where the user to be removed is*/
				for (; cursor != NULL; cursor = cursor -> next_acct) {
					if ((!strcmp(cursor->next_acct->name, acctToBeRemoved)) && (cursor->next_acct->next_acct == NULL)) { /*User is at the end*/
						// printf("Deleting From the end of the list!\n");
						cursor -> next_acct = NULL; /*Remove the last node*/
						return 1;
					} else if ((!strcmp(cursor->next_acct->name, acctToBeRemoved)) && (cursor->next_acct->next_acct != NULL)) { /*In the middle of the list*/
						// printf("Deleting from the middle of the list!\n");
						cursor -> next_acct = cursor -> next_acct -> next_acct;
						return 1;
					}
				}
			}
		}
		return 0;
	}
	return 0;
}

int checkAcct(Accts **head, char *name) {
	if (*head != NULL) {
		Accts *cursor = *head;
		for (; cursor != NULL; cursor = cursor -> next_acct) {
			if (!strcmp(name, cursor->name)) {
				return 1;
			}
		}
	} else {
		fprintf(stderr, "The account list is empty!\n");
		return 0;
	}

	return 0;
}

int addChat(Chats **head, Chats *chat) {
	if (*head != NULL) {
		Chats *cursor = *head;
		if (!checkChat(head, chat->name)) {
			for (; cursor != NULL; cursor = cursor -> next_chat) {
				if (cursor -> next_chat == NULL) {
					cursor -> next_chat = chat;
					chat -> next_chat = NULL;
					return 1;
				}
			}
		}
	} else {
		chat -> next_chat = NULL;
		*head = chat;
		return 1;
	}
	return 0;
}

int removeChat(Chats **head, char *chatToBeRemoved) {
	if (*head != NULL) {
		Chats *cursor = *head;
		if (checkChat(head, chatToBeRemoved)) {	/*Must be in the list to remove it*/
			if (!strcmp(cursor->name, chatToBeRemoved)) {	/*If the user is at the head of the list*/
				// printf("Deleting from the head of the list\n");
				*head = cursor -> next_chat; /*Make the next user the new head*/
				return 1;
			} else {
				/*loop through and find where the user to be removed is*/
				for (; cursor != NULL; cursor = cursor -> next_chat) {
					if ((!strcmp(cursor->next_chat->name, chatToBeRemoved)) && (cursor->next_chat->next_chat == NULL)) { /*User is at the end*/
						// printf("Deleting From the end of the list!\n");
						cursor -> next_chat = NULL; /*Remove the last node*/
						return 1;
					} else if ((!strcmp(cursor->next_chat->name, chatToBeRemoved)) && (cursor->next_chat->next_chat != NULL)) { /*In the middle of the list*/
						// printf("Deleting from the middle of the list!\n");
						cursor -> next_chat = cursor -> next_chat -> next_chat;
						return 1;
					}
				}
			}
		}
		return 0;
	}
	return 0;
}

int checkChat(Chats **head, char *name) {
	Chats *cursor = *head;
	for (; cursor != NULL; cursor = cursor -> next_chat) {
		if (!strcmp(name, cursor->name)) {
			return 1;
		}
	}
	return 0;
}

int addUser(Users **head, Users *user) {
	if (*head != NULL) {
		Users *cursor = *head;
		if (!checkUser(head, user->name)) {
			for (; cursor != NULL; cursor = cursor -> next_user) {
				if (cursor -> next_user == NULL) {
					cursor -> next_user = user;
					user -> next_user = NULL;
					return 1;
				}
			}
		}
	} else {
		user -> next_user = NULL;
		*head = user;
		return 1;
	}
	return 0;
}

int removeUser(Users **head, char *userToBeRemoved) {
	if (*head != NULL) {
		Users *cursor = *head;
		if (checkUser(head, userToBeRemoved)) {	/*Must be in the list to remove it*/
			if (!strcmp(cursor->name, userToBeRemoved)) {	/*If the user is at the head of the list*/
				// printf("Deleting from the head of the list\n");
				*head = cursor -> next_user; /*Make the next user the new head*/
				return 1;
			} else {
				/*loop through and find where the user to be removed is*/
				for (; cursor != NULL; cursor = cursor -> next_user) {
					if ((!strcmp(cursor->next_user->name, userToBeRemoved)) && (cursor->next_user->next_user == NULL)) { /*User is at the end*/
						// printf("Deleting From the end of the list!\n");
						cursor -> next_user = NULL; /*Remove the last node*/
						return 1;
					} else if ((!strcmp(cursor->next_user->name, userToBeRemoved)) && (cursor->next_user->next_user != NULL)) { /*In the middle of the list*/
						// printf("Deleting from the middle of the list!\n");
						cursor -> next_user = cursor -> next_user -> next_user;
						return 1;
					}
				}
			}
		}
		return 0;
	}
	return 0;
}

Users* getUserByFd(Users **head, int fd) {
	if (*head != NULL) {
		Users *cursor = *head;
		for (; cursor != NULL; cursor = cursor -> next_user) {
			if (cursor -> socketfd == fd) {
				return cursor;
				break;
			}
		}
		fprintf(stderr, "That fd is not in the list!\n");
		return NULL;
	} else {
		fprintf(stderr, "The user's list is empty!\n");
		return NULL;
	}
}

Chats* getChatByFd(Chats **head, int fd) {
	if (*head != NULL) {
		Chats *cursor = *head;
		for (; cursor != NULL; cursor = cursor -> next_chat) {
			if (cursor -> socketfd == fd) {
				return cursor;
				break;
			}
		}
		fprintf(stderr, "That fd is not in the list!\n");
		return NULL;
	} else {
		fprintf(stderr, "The user's list is empty!\n");
		return NULL;
	}
}


int checkUser(Users **head, char *name) {
	if (*head != NULL) {
		Users *cursor = *head;
		for (; cursor != NULL; cursor = cursor -> next_user) {
			if (!strcmp(name, cursor->name)) {
				return 1;
			}
		}
	} else {
		fprintf(stderr, "The user list is empty!\n");
		return 0;
	}
	return 0;
}
int get_user_fd(Users **head, char *name) {
	if (*head != NULL) {
		Users *cursor = *head;
		for (; cursor != NULL; cursor = cursor -> next_user) {
			if (!strcmp(name, cursor->name)) {
				return cursor->socketfd;
			}
		}
	} else {
		fprintf(stderr, "The user list is empty!\n");
		return 0;
	}
	return 0;

}
int get_chat_fd(Chats **head, char *name) {
	if (*head != NULL) {
		Chats *cursor = *head;
		for (; cursor != NULL; cursor = cursor -> next_chat) {
			if (!strcmp(name, cursor->name)) {
				return cursor->socketfd;
			}
		}
	} else {
		fprintf(stderr, "The chat list is empty!\n");
		return 0;
	}
	return 0;
}
int get_chat_fd_with_pid(Chats **head, pid_t pid) {
	if (*head != NULL) {
		Chats *cursor = *head;
		for (; cursor != NULL; cursor = cursor -> next_chat) {
			if (cursor->pid == pid) {
				return cursor->socketfd;
			}
		}
	} else {
		fprintf(stderr, "The chat list is empty!\n");
		return 0;
	}
	return 0;
}

int tokenize(char *buf, char **argv) {
	const char* delim = {" "};


	int argc = 0;

	while ((argv[argc] = strtok(buf, delim)) != NULL) {
		argc++;/*increment argc*/
		buf = buf + strlen(argv[argc - 1]) + 1; /*set buffer to correct position*/
		/*Remove any white spaces between words*/
		while (*buf != '\0' && (*buf == ' '))
			buf++;
	}

	argv[argc] = NULL;


	return argc;
}




int get_ip() {
	printf("%s\n", "\n\x1B[1;37mClient can connect to: \x1B[0m");
	int sock, i;
	struct ifreq ifreqs[20];
	struct ifconf ic;

	ic.ifc_len = sizeof ifreqs;
	ic.ifc_req = ifreqs;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(1);
	}

	if (ioctl(sock, SIOCGIFCONF, &ic) < 0) {
		perror("SIOCGIFCONF");
		exit(1);
	}

	for (i = 0; i < ic.ifc_len / sizeof(struct ifreq); ++i)
		printf("\t%s: \x1B[1;36m%s\x1B[0m\n", ifreqs[i].ifr_name,
		       inet_ntoa(((struct sockaddr_in*)&ifreqs[i].ifr_addr)->sin_addr));

	close(sock);
	return 0;
}


// void read_stuff(char *message, int clientfd){
// 	printf("%s\n", "READING");
// 	memset(message, 0, strlen(message));
// 	// read(clientfd, message, MAXLINE);/*read what the client replies*/

// 	char last_char;
// 	char *cursor;
// 	int rv;
// 	cursor = message;
// 	while (strstr(message, "\r\n\r\n") == NULL) {
// 		printf("%s\n", "INSIDE WHILE");
// 		rv = read(clientfd, &last_char, 1);
// 		if (rv == -1){/*no more*/
// 		printf("%s\n", "ERROR READING");
// 			break;
// 		}
// 		printf("%s\n", message);
// 		*cursor = last_char;
// 		cursor++;
// 	}
// 	printf("%s\n", "EXTITING read_stuff");
// }

int read_stuff(char *message, int clientfd){
	// printf("%s\n", "READING");
	memset(message, 0, strlen(message));
	// read(clientfd, message, MAXLINE);/*read what the client replies*/

	char last_char;
	char *cursor;
	int rv;
	cursor = message;
	while (strstr(message, "\r\n\r\n") == NULL) {
		// printf("%s\n", "INSIDE WHILE");
		rv = read(clientfd, &last_char, 1);
		if (rv == -1){/*no more*/
			return 1;
		}
		// printf("%s\n", message);
		*cursor = last_char;
		cursor++;
	}
	return 0;
	// printf("%s\n", "EXTITING read_stuff");
}