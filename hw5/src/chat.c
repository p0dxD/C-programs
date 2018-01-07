#include "hw5.h"
void read_message_chat(int fd);

void chat_handler(int sig);
int clientfd;
int main(int argc, char **argv) {
	char buf[MAXLINE];
	if (argc != 2) {
		exit(1);
	}

	if ((clientfd = atoi(argv[1])) == 0 ) {
		exit(1);
	}

	fd_set read_set, ready_set;
	FD_ZERO(&read_set);/*clear the set*/
	FD_SET(STDIN_FILENO, &read_set);/*add file descriptor*/
	FD_SET(clientfd, &read_set);
	if (Signal(SIGSEGV, chat_handler) == SIG_ERR){}
	while (1) {
		ready_set = read_set;
		Select(clientfd + 1, &ready_set, NULL, NULL, NULL);
		if (FD_ISSET(STDIN_FILENO, &ready_set)) {
			if (!Fgets(buf, MAXLINE, stdin))
				exit(0);
			fputs("\033[A\033[2K", stdout);
			buf[strlen(buf)] = '\0'; /*Null terminate it*/
			write(clientfd, buf, strlen(buf)+1);
			// if (strstr(buf, "hi") != NULL) {
			// 	printf("%s\n", "display users: ....");
			// 	if (write(clientfd, "HI\r\n\r\n", strlen("HI\r\n\r\n")) > 0) { /*send Wolfie*/
			// 		printf("%s\n", "Success");
			// 		// close(clientfd);
			// 	}
			// }
		}

		if (FD_ISSET(clientfd, &ready_set)) {
			read_message_chat(clientfd);
		}
		/*end while*/

	}

	return 0;
}

void read_message_chat(int fd) {
	// printf("\xB1[2J");
	char last_char = ' ';
	char *cursor;
	char buf[MAXLINE];
	// char msg[MAXLINE];
	memset(buf, 0, MAXLINE);
	// memset(msg, 0, MAXLINE);
	int rv;
	cursor = buf;
	while (last_char != '\0') {
		// printf("%c\n", last_char);
		rv = read(fd, &last_char, 1);
		if (rv < 0 ) {
			break;
		}
		// printf("%s char got %c \n", "inside while", last_char);
		*cursor = last_char;
		cursor++;
	}
	*cursor = '\0';

	
	if ((strstr(buf, "User not available ERR02") != NULL)) {
		printf("%s\n", "inside user not avaiable");
		printf("%s\n", buf);
		printf("%s\n", "Press enter to exit.");
		getchar();
		write(clientfd, "/close", strlen("/close") + 1);

	}
	printf("\33[2K\r");
	printf("%s\n", buf);
	// pause();
}

void chat_handler(int sig){
	printf("%s\n", "test");
	pause();
	// printf("%s\n", "caught close");
	// if (sig == SIGKILL){
	// 	pause();
	// 	// write(clientfd, "/close", strlen("/close") + 1);
	// }
}