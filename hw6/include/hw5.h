#ifndef __HW5_H__
#define __HW5_H__
// #define _GNU_SOURCE
// #define _GNU_SOURCE
// #include <stdio.h>
// #include <pthread.h>
#include "csapp.h"
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <string.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <assert.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include "sfwrite.h"

#define AUTH "AUTH"
#define IAMNEW "IAMNEW"
#define HINEW "HINEW"
#define NEWPASS "NEWPASS"
#define WOLFIE "WOLFIE \r\n\r\n"
#define WOLFIE_BACKWARDS "EIFLOW \r\n\r\n"
#define IAM_MESSAGE "IAM"
#define HI_MESSAGE "HI"
#define UOFF "UOFF"
#define TIME_MESSAGE "EMIT"
#define TIME_MESSAGE_SEND "TIME \r\n\r\n"
#define BYE_MESSAGE "BYE \r\n\r\n"
#define ERR_MESSAGE "ERR 00 USER NAME TAKEN \r\n\r\n"
#define ERR_MESSAGE_USER_NOT_AVAILABLE "ERR 01 USER NOT AVAILABLE \r\n\r\n"
#define ERR_MESSAGE_USER_TAKEN "ERR 00 USER NAME TAKEN \r\n\r\n"
#define ERR_BAD_PASSWORD "ERR 02 BAD PASSSWORD \r\n\r\n"
#define NEWPASS_BACKWARDS "SSAPWEN \r\n\r\n"
#define LISTU "LISTU \r\n\r\n"
#define PASS_BACKWARDS "SSAP \r\n\r\n"
#define END_STRING "\r\n\r\n"

typedef struct Users Users;
struct Users {
	char name[MAXLINE];
	int socketfd;
	char* ip;
	time_t timeConnected;
	Users *next_user;
};

typedef struct Accts Accts;
struct Accts {
	char name[MAXLINE];
	char saltValue[MAXLINE];
	char hashedPass[MAXLINE];
	Accts *next_acct;
};

typedef struct Chats Chats;
struct Chats {
	char name[MAXLINE];
	int socketfd;
	pid_t pid;
	Chats *next_chat;
};

void sha256(char string[MAXLINE], char outputBuffer[65]);
void echo(int connfd);
void removeFromArray(struct epoll_event *events, int index);
void serverShutdown(void);
void printHelp(void);
int writeMsgToFile(char *buf, char *msg, char *name, char *filename, pthread_mutex_t count_mutex);
void printUsers(void);
Users* getHead(void);
char* getDateTimeStamp();
char* getIPAddress(int fd);
time_t getEndTime(time_t beginTime);
void read_audit(char *fileName);
int addAcct(Accts **head, Accts *acct);
int removeAcct(Accts **head, char *acctToBeRemoved);
int checkAcct(Accts **head, char *name);
int addChat(Chats **head, Chats *chat);
int removeChat(Chats **head, char *chatToBeRemoved);
int checkChat(Chats **head, char *name);
int removeUser(Users **head, char *userToBeRemoved);
int addUser(Users **head, Users *user);
Users* getUserByFd(Users **head, int fd);
int checkUser(Users **head, char *name);
void read_message(int fd, int i);
char *strrev(char *str);
void reply_message(char *message, char *dest, char *name);
void send_message(int connfd, char *name);
int tokenize(char *buf, char **argv);
int get_ip();
void Flock(int fd, int operation);
void addevent(int fd);
void *comm_th(void *vargp);
void *login_thread(void *vargp);
void *comm_thread(void *vargp);
void children_handler(int sig);
void ctrl_c_handler(int signo);
void client_shutdown();
void ctrl_c_handler_client(int signo);
int launch_chat(int fd, char *name, int size);
void init_connexion(int clientfd, char* name, int verbose, int create);
int create_msg(char *buf, char *msg, char *name);
int get_user_fd(Users **head, char *name);
int get_chat_fd(Chats **head, char *name);
Chats* getChatByFd(Chats **head, int fd);
void read_message_from_chat(int fd, char *name);
int get_chat_fd_with_pid(Chats **head, pid_t pid);
void kill_chat(int fd, int kill_it);
void print_users_client(char *buf);
/*In server*/
void write_accounts();
void read_accounts();

void print_accounts(void) ;
void *login_thread_new(void *vargp);
// void read_stuff(char *message,int  clientfd);
int read_stuff(char *message, int clientfd);
void printHelpClient();

typedef struct {
	int *buf; /*Buffer array*/
	int n;/*Maximum number */
	int front; /*Buff(front+1)%n]*/
	int rear; /*buf[rear%n] is last item*/
	sem_t mutex; /*Protects accesses to buf*/
	sem_t slots; /*Counts available slots*/
	sem_t items; /*Counts available items*/
} sbuf_t;

void sbuf_init(sbuf_t *sp, int n);
void sbuf_deinit(sbuf_t *sp);
// void sbuf_insert(sbuf_t *sp);
void sbuf_insert(sbuf_t *sp, int item);
int sbuf_remove(sbuf_t *sp);
#endif
