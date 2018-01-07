/*Note: sudo apt-get install lib32ncurses5-dev*/
#include "320sh.h"
History* head = NULL;
int historyCounter = 0;
int debug = 0;
int bg = 0;
void  INThandler(int sig);
int main (int argc, char ** argv, char **envp) {

  int opt;
  while((opt = getopt(argc, argv, "d")) != -1) {

    switch(opt) {
      case 'd':
      debug = 1;
      break;
      default:
      break;
    }
  }
  read_history();


  int finished = 0;
  char *prompt = "320sh> ";
  char cmd[MAX_INPUT];

  while (!finished) {
    char *cursor;
    char last_char;
    int rv;
    int count;
    int exec = 1;/*if they send in "^c" we dont execute*/
    int moved = 0;
    int string_cursor = 0;
    int i = 0;


      /*adds the current directory to the prompt*/
      rv = write(1, "[", 1);
      rv = write(1, get_current_dir(), strlen(get_current_dir()));
      rv = write(1, "]", 1);
      rv = write(1, prompt, strlen(prompt));

      while(MAX_INPUT > i){
        cmd[i++] = '\0';
      }
    // if (!rv) {
    //   finished = 1;
    //   break;
    // }
// int test;s
    // read and parse the input
    // printf("%s\n", "outside");
    for(rv = 1, count = 0, cursor = cmd, last_char = 1;rv && (++count < (MAX_INPUT-1)) && (last_char != '\n'); cursor++) {
    // printf("\nCMD START%s\n", cmd);
    rv = read(0, &last_char, 1);/*writes input into cmd*/
    // last_char = *cursor;/*cursor pointing at the end of what was written*/
    // test = getch();
    // printf("%X\n", test);
    if(last_char =='\"'){
      // printf("last cmahar%c\n", last_char);
      // printf("iside\n");
      int broken = 0;
      string_cursor++;
      *cursor = last_char;
      cursor++;
       write(1, &last_char, 1);/*write to screen*/
      rv = read(0, &last_char, 1);
      while(last_char!= '\"'){

        if(last_char == '\n'){
          write(1,"\n>",2);
          goto read_next;
          // write(1, &last_char, 1);
        }else if(last_char == 3){
          write(1, "^c", 2);/**write that to screen and end current input, does not execute it*/
          exec = 0;/*we dont want to execute what ever they put in*/
          cursor = cmd;/*pointer back to start*/
          broken =1;
          break;/*break from the loop*/
        }else if(last_char == 0x1b){
          // write(1, "\b \b", 3);/*clear ""*/
          // write(1,"\n>",2);
          cursor = cursor-1;

          goto curso;
          // break;
        }else if(last_char == 0x7F && !moved){/*backspace*/
          if(cursor != cmd){
            *cursor = '\0';
            cursor = cursor-1;/*add two because the loop will add one to it and keep it in the same position*/
            *cursor = '\0';
            // cursor = cursor-1;/*add two because the loop will add one to it and keep it in the same position*/
            count-=2;
            write(1, "\b \b", 3);/*go back one, write space, go back another*/
            // printf("\nCMD%s\n", cmd);
          }else{
            cursor = cursor-1;/*set pointer only one back due to cursor being ++ on for loop*/
            count--;
          }

        }
        // rv = read(0, &last_char, 1);/*read next*/
        write(1, &last_char, 1);
        *cursor = last_char;
        string_cursor++;
        cursor++;
        read_next:
        rv = read(0, &last_char, 1);/*read next*/
        count++;
      }/*end while*/
      if(broken){
        break;
      }
      count++;

      write(1, &last_char, 1);
      *cursor = last_char;
      // cursor++;
      continue;
    }
    if(last_char == 3) {/*checks for ctrl-c*/
      write(1, "^c", 2);/**write that to screen and end current input, does not execute it*/
      exec = 0;/*we dont want to execute what ever they put in*/
      cursor = cmd;/*pointer back to start*/
      break;/*break from the loop*/
    // }else if(test == KEY_LEFT){
      // printf("%s\n", "okay");
    }else if(last_char == 0x1b){
      curso:
      // printf("%s\n", "escape");
      // cursor = cursor -1;/*we dont want what it wrote*/
      count--;
      rv = read(0, &last_char, 1);/*writes input into cmd*/
      // last_char = *cursor;/*cursor pointing at the end of what was written*/
      if(last_char == '['){
        // cursor = cursor -1;/*we dont want what it wrote*/
      rv = read(0, &last_char, 1);/*writes input into cmd*/
      // last_char = *cursor;/*cursor pointing at the end of what was written*/
      // cmd[strlen(cmd)-1] ='\0';
      // printf("Last char %c\n", last_char);
        if(last_char == 'A'){/*up key*/
          // cursor = cursor -1;/*we dont want what it wrote*/
          if(head!=NULL){
            size_t diff = cursor - cmd;
            // printf("diff %ld\n", diff);
            while(diff>0){
              *cursor = '\0';
              cursor = cursor-1;/*add two because the loop will add one to it and keep it in the same position*/
              // printf("cursor %p cmd %p\n", cursor, cmd);
              // printf("diff %ld\n", diff);
              write(1, "\b \b", 3);
              diff--;
            }
            // rv = write(1, "[", 1);
            // rv = write(1, get_current_dir(), strlen(get_current_dir()));
            // rv = write(1, "]", 1);
            // rv = write(1, prompt, strlen(prompt));


            cursor = cmd;/*bring it back to begining*/
            count = 0;
            char *al = get_prev();
            // printf("CONTENTS %s\n", al);
            write(1, al, strlen(al));
            cursor = cursor+strlen(al)-1;/*set pointer only one back due to cursor being ++ on for loop*/
            string_cursor = strlen(al);/*Set cursor at end*/
            count+=strlen(al)-1;
            strcpy(cmd, al);/*copy contents into cmd*/
            // printf("\nCONTENTS cmd %s\n", cmd);
          }else{
            cursor = cursor -1; /*else it doesnt write the A*/
            count--;
          }
          continue;
        }
          // write(1, "up key", 7);
          else if(last_char == 'B'){/*down key*/
            // cursor = cursor -1;/*we dont want what it wrote*/
            // printf("%s\n", get_prev());
            if(head!=NULL){
              // printf("cursor %p cmd %p\n", cursor, cmd);
              size_t diff = cursor - cmd;
              // printf("diff %ld\n", diff);
              while(diff>0){
                *cursor = '\0';
                cursor = cursor-1;/*add two because the loop will add one to it and keep it in the same position*/
                // printf("diff %ld\n", diff);
                write(1, "\b \b", 3);
                diff--;
              }
              // rv = write(1, "[", 1);
              // rv = write(1, get_current_dir(), strlen(get_current_dir()));
              // rv = write(1, "]", 1);
              // rv = write(1, prompt, strlen(prompt));
              cursor = cmd;/*bring it back to begining*/
              count = 0;
              char *al = get_next();
              write(1, al, strlen(al));
              cursor = cursor+strlen(al)-1;/*set pointer only one back due to cursor being ++ on for loop*/
              string_cursor = strlen(al)-1;/*Set cursor at end*/
              count+=strlen(al)-1;
              // printf("CONTENTS %s\n", al);
              strcpy(cmd, al);/*copy contents into cmd*/
              // printf("\nCONTENTS cmd %s\n", cmd);
            }else{
              // *cursor = '\0';
              cursor = cursor -1;/*not make it write the B*/
              count--;
            }
          continue;
        }
          else if(last_char == 'C'){/*move right*/
            // cmd[strlen(cmd)-1] ='\0';
            // printf("CMD %ld %s %d\n", strlen(cmd), cmd, string_cursor);
            if(string_cursor<strlen(cmd)){

            // size_t diff = cursor - cmd;
            // printf("length %ld count %d\n ", strlen(cmd),count);
            // printf("Size %ld\n", size_of_cmd);
              write(1, "\033[1C", 4);/*move foward*/
              // cursor = cursor - 1;
              // *cursor = '\0';
              string_cursor++;
            cursor = cursor+1;/*set pointer only one back due to cursor being ++ on for loop*/
            moved = 1;/*set moved*/
          }
          cursor = cursor-1;/*set pointer only one back due to cursor being ++ on for loop*/
          // cmd[strlen(cmd)-1] =' ';
          // printf("%s\n", cmd);
          continue;
        }
          else if(last_char == 'D'){/*move left*/
            // printf("Count %d\n", count);
            // cmd[strlen(cmd)-1] ='\0';
            // printf("CMD %ld %s %d\n", strlen(cmd), cmd, string_cursor);
            if(cursor > cmd){
              // printf("cursor %p cmd %p\n", cursor, cmd);
            write(1, "\b", 1);
            // *cursor = '\0';/*ADDED*/
            cursor = cursor - 1;
            string_cursor--;
            moved = 1;/*set moved*/
          }
          cursor = cursor-1;/*set pointer only one back due to cursor being ++ on for loop*/
          continue;
        }
      }/*end if of '['*/
        // printf("%s\n", "okay getting somewhere");
    }else if(last_char == '\t'){
      /*Here do tab autocompletion or what ever*/
      cursor = cursor-1;/*not move it*/
      count--;
      continue;
    }else if(last_char == 0x7F && !moved){/*backspace*/
      if(cursor != cmd){
        *cursor = '\0';
        cursor = cursor-1;/*add two because the loop will add one to it and keep it in the same position*/
        *cursor = '\0';
        cursor = cursor-1;/*add two because the loop will add one to it and keep it in the same position*/
        count-=2;
        write(1, "\b \b", 3);/*go back one, write space, go back another*/
        // printf("\nCMD%s\n", cmd);
      }else{
        cursor = cursor-1;/*set pointer only one back due to cursor being ++ on for loop*/
        count--;
      }

    }else {
      if(moved){
        if(last_char == '\n'){
          write(1, "\n", 1);
          // cmd[strlen(cmd)] = '\0';

          break;
        }
        if(last_char == 0x7F&& cursor!=cmd){
          // size_t diff = cursor - cmd;
          size_t diff =100;
          // printf("diff %ld\n", diff);
          // // write(1, " ", 1);
          while(diff>0){
            // printf("diff %ld\n", diff);
          write(1, "\033[K", 3);/*move foward*/
            write(1, "\b \b", 3);
            diff--;
          }
          rv = write(1, "[", 1);
          rv = write(1, get_current_dir(), strlen(get_current_dir()));
          rv = write(1, "]", 1);
          rv = write(1, prompt, strlen(prompt));

          char* dest2 = (char*)malloc((strlen(cmd))*sizeof(char));
          delete_char(dest2, cmd, string_cursor-1);
          strcpy(cmd,dest2);
          int loo = string_cursor-1;
          cursor = cmd + string_cursor-1;/*bring it back to begining*/
          // printf("\nCMD %s\n", cmd);
          write(1, cmd, strlen(cmd));
          while(loo>0){
            write(1, "\b", 1);
            loo--;
          }
          // string_cursor--;
          free(dest2);
          continue;
        }
        // printf("\n%s %s\n", "inside move",cmd);
        size_t diff = cursor - cmd;
        // printf("diff %ld\n", diff);
        while(diff>0){
          // printf("diff %ld\n", diff);
          write(1, "\b \b", 3);
          diff--;
        }

        // printf("\nCMD %s\n\n", cmd);
        char b[2] ="\0";
         b[0] = last_char;
        char* dest = (char*)malloc((strlen(cmd)+2)*sizeof(char));
        // size_t diff = cursor - cmd;
        // printf("CURSOR %ld\n", diff);
        insert(dest,cmd,b,string_cursor);
        // printf("Dest %s\n", dest);
        strcpy(cmd,dest);
        // printf("CMD %s\n", cmd);
        int loo = string_cursor-1;
        cursor = cmd + string_cursor;/*bring it back to begining*/
        write(1, cmd, strlen(cmd));
        while(loo>0){
          write(1, "\b", 1);
          loo--;
        }
        free(dest);
      }else{/*we jsut write it here*/
      string_cursor++;
      *cursor = last_char;
       write(1, &last_char, 1);/*write to screen*/
      //  printf("\nCURSOR LOCATION%d\n", string_cursor);
     }
    }

  }
  // printf("\nCOUNT %d %s %ld\n", count,cmd ,strlen(cmd));

  // printf("cmd %s Size %ld %d\n", cmd, strlen(cmd), strcmp(cmd,"quit"));

  // printf("cursor %s, char %c,rv %d,count %d\n", cursor, last_char, rv, count);
  if (!rv) {
    finished = 1;
    break;
  }

// printf("CMD after %s\n", cmd);
  // Execute the command, handling built-in commands separately
  // Just echo the command line for now
  /*Execute if they didnt press ctrl-c*/
  if(exec){
    eval(cmd);/*Send it to be evaluated*/
    exec = 1;/*Make it exec for next else it will not be*/
  }else
    write(1, "\n", 1);/*we write a new line else 320sh prints on same line*/

}
return 0;
}
/*******************************************************************************/
void insert(char* dest ,char *strA, char *strB, int offset){

  strncpy(dest,strA,offset);
  dest[offset] = '\0';
  // printf("%s\n",strB);
  strcat(dest,strB);
  strcat(dest,strA+offset);

}
// abc
void delete_char(char *dest, char *str, int i) {
  // char strF[MAX_INPUT];
  strncpy(dest,str,i);
  dest[i] = '\0';
  str = str + i+1;
  strcat(dest,str);

}
/*******************************************************************************/
void sig(int sig){
  Sio_puts("^C");
  // _exit(0);
}
int status_child;
void eval(char *cmd){
  char *argv[MAXARGS];/*The max arguments it can have 128 for some reason*/
  char *args[MAXARGS];/*The max arguments it can have 128 for some reason*/
  char buf[MAXLINE];/*where the string is at*/
  int status, argc;/*keeps track of error*/
  int greaterThan = 0;
  int lessThan = 0;
  int input_fd = 1, output_fd = -1;
  pid_t pid;


  if(strcmp(cmd, "\n")!=0 && cmd != NULL){/*fixes issue with user entering jsut enter*/
  strcpy(buf, cmd);/*copy into buffer to modify it*/
  argc = tokenize(buf, argv);/*break it into its components*/

  // if(!strcmp(argv[2], "o")){
  //   printf("Argv[i] is equal to o without space.\n" );
  // } else if(!strcmp(argv[2], "o ")){
  //   printf("Argv[i] has a space\n" );
  // }s
  int i = 0;
  // printf("Argc: %d\n", argc);
  // printf("First argv: %s\n", argv[0]);
  while(i < argc){
    if(strcmp(argv[i], "<") == 0){
      // printf("Less than:\n" );
      // printf("Argv being copied: %s\n", argv[i]);
      // args[i] = argv[i];
      // printf("i inside the loop: %d\n", i);
      i++;
      lessThan = 1;
      break;
    }
    if(strcmp(argv[i], ">") == 0){
      // printf("Greater than:\n" );
      // printf("Argv being copied: %s\n", argv[i]);
      // args[i] = argv[i];
      // printf("i inside the loop: %d\n", i);
      greaterThan = 1;
      i++;
      break;
    }
    args[i] = argv[i];
    i++;
  }

  args[i] = NULL;
//   int j = 0;
//   while(args[j]!=NULL){
//     printf("args[%d] %s\n",j, args[j]);
//     j++;
//
//   }
//   while(argv[i]!=NULL){
//   printf("argv[%d] %s\n",i, argv[i]);
//   i++;
// }
//   exit(0);

  // printf("I after loop: %d\n", i);
  // printf("Entire argv list: %s %s %s\n", argv[0], argv[1], argv[2]);
void sig(int sig);
void handler2(int sig);

  signal(SIGINT, sig);
  signal(SIGCHLD, handler2);
  if((status = shell_command(argv, argc)) == -1){/*if -1, its not a built-in*/
    // printf("%s %s\n", "lets fork it see if its there", argv[0]);
// int pfd[2];   
     // if(pipe(pfd) == -1)  {}                  /* Create the pipe */
         // errExit("pipe");                          /* Pipe file descriptors */
    if((pid = Fork()) == 0){
      if((stat_check(args[0])==0)||(*args[0] == '/')||strstr(args[0], "./")){/*file found*/
  // signal(SIGINT, INThandler);
        // printf("%s\n", "it is");
        // printf("> %d and < %d\n", greaterThan, lessThan);
        // if(!strcmp(argv[i], "o")){
        //   printf("Argv[i] is equal to o without space.\n" );
        // } else if(!strcmp(argv[i], "o ")){
        //   printf("Argv[i] has a space\n" );
        // }
if(greaterThan || lessThan){
        // printf("File destination: %s\n", argv[i]);
        char dir[MAX_INPUT];
        strcpy(dir, get_current_dir());
        strcat(dir, "/");
        strcat(dir, argv[i]);
        // printf("Directory: %s\n", dir);
        if((output_fd = open(dir, O_CREAT | O_WRONLY,
          S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) < 0) {
            fprintf(stderr,"%s\n", "Error opening up file.");
          }
          if(greaterThan == 1){
            Dup2(output_fd, input_fd);
          } else if(lessThan == 1){
            Dup2(1, output_fd);
          }

          close(output_fd);
        }
        if(debug){
        fprintf(stderr, "RUNNING: %s\n", argv[0]);
      }
      //    if (close(pfd[1]) == -1){}            /* Write end is unused */
      // //        // errExit("close - child");
      //   Dup2(pfd[0],STDIN_FILENO); 
      //   printf("d %d\n", pfd[0]);
      //   close(pfd[0]); /* close the fd of read end of the pipe */
      // int n;
      // read(0,&n,1);
        write(1,"child",strlen("child"));
      sio_putl((long)getpgrp());
        if(execvp(args[0], args) == -1){
            printf("%s: Command not found.", argv[0]);
          _exit(1);
      }
    // }
      // printf("%s\n", "error");
    //   int i = 1, redirect = 0;
    //   while(i < argc){
    //   if(strstr(argv[i],'>')){
    //     FILE *file = fopen(argv[i+1], "w+");
    //
    //     }
    //   i++;
    // }
    }else{
      fprintf(stderr, "Command not found.%s\n", cmd);
      _exit(1);/*exit*/

    }
      _exit(0);/*exit good*/
  }else{
      // if (close(pfd[0]) == -1) {}
      // void handle_SIGINT();
    if(!bg){/*not a background lets wait for it*/
    do {
      //   printf("d %d\n", pfd[1]);
      // write(pfd[1], "hello", strlen("hello"));
      //   close(pfd[1]);
      printf("group %d\n", getpgrp()); 
      // pause();
      waitpid(pid, &status_child, WUNTRACED);/*reap*/
    } while (!WIFEXITED(status_child) && !WIFSIGNALED(status_child));
      bg = 0;/*back to zero*/
  }else{printf("%s\n", "background"); bg = 0;}
      // if (close(pfd[1]) == -1) {}
      // if (close(pfd[0]) == -1) {}
    if(debug){
    fprintf(stderr, "ENDED: %s (ret=%d)\n", argv[0], status_child);
  }
  }

  }else if(status > 0){/*It returned an error*/
    fprintf(stderr, "Error executing %s\n", argv[0]);
  }/*else it was 0 all went well*/
}/*end string compare*/
}
void handler2(int sig){
  int olderrno = errno;
  while(waitpid(-1, NULL, 0) > 0){
    Sio_puts("reaped");
  }
  if(errno != ECHILD)
    Sio_error("waitpid error");
  // Sleep(1);
  errno = olderrno;
}

void  INThandler(int sig)
{
     char  c;

     signal(sig, SIG_IGN);
     printf("OUCH, did you hit Ctrl-C?\n"
            "Do you really want to quit? [y/n] ");
     c = getchar();
     if (c == 'y' || c == 'Y')
          exit(0);
     else
          signal(SIGINT, INThandler);
     getchar(); // Get new line character
}
// void redirect_file(int redirect,char **argv,int input_fd,int output_fd, int i){
//   if(redirect){
//           printf("LOL %s\n", argv[i]);
//           char dir[MAX_INPUT];
//           strcpy(dir, get_current_dir());
//           strcat(dir, "/");
//           strcat(dir, argv[i]);
//           if((output_fd = open(dir, O_CREAT | O_WRONLY,
//             S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) < 0) {
//               printf("%s\n", "error");
//               // fprintf(standardout, "Failed to open the file %s\n", input_path);
//             }
//             printf("file des %d\n", output_fd);
//             Dup2(output_fd,input_fd);
//   }
// }
int tokenize(char *buf, char **argv){
  const char* delim = {" "};
  char *ptr;
  /*remove any trailing spaces*/
  while(*buf != '\0' && (*buf == ' '))
    buf++;

  int argc = 0;
/*Removes new line before getting into the loop*/
  if((ptr = strchr(buf, '\n')) != NULL){
    *ptr = '\0';/*Make it a null instead*/
  }
// printf("buf %s\n", buf);
  enqueue(buf);
while((argv[argc] = strtok(buf, delim))!=NULL){
  argc++;/*increment argc*/
  buf = buf + strlen(argv[argc-1])+1;/*set buffer to correct position*/
  /*Remove any white spaces between words*/
  while(*buf != '\0' && (*buf == ' '))
    buf++;

}


argv[argc] = NULL;

if (((*argv[argc-1] == '&'))){
      bg = 1;
      argv[--argc] = NULL;
    }

  printf("%s %d %s\n", "bg", bg, argv[argc-1]);
return argc;
}

/*Method returns > 0 if error, 0 < if not a command or 0 if all went well*/
int shell_command(char **argv, int argc){
  // char *path;
    // printf("INSIDE %s Size %ld %d\n", argv[0], strlen(argv[0]), strcmp(argv[0],"quit"));
  if(!strcmp(argv[0],"exit")){
    enqueue("exit");/*write history*/
    write_history();
    // printf("%s\n", "exit");
    exit(0);
  }
  if(!strcmp(argv[0],"cd"))
    return cd_exec(argv);
  if(!strcmp(argv[0],"pwd"))
    return pwd_exec();
  if(!strcmp(argv[0], "echo"))
    return echo_exec(argv, argc);
  if(!strcmp(argv[0], "set"))
    return set_exec(argv, argc);
  if(!strcmp(argv[0], "history"))
    history_exec();
  if(!strcmp(argv[0], "clear-history"))
    clearHistory_exec();
  if(!strcmp(argv[0], "help"))
    help_exec();
  return -1;
}

/**/
void help_exec(){

  fprintf(stderr,                                                                                                     \
      "\nC-coders 320 Shell, version 4.3.42(1)-release (x86_64-pc-linux-gnu) \n"                                                                           \
      "\n"                                                                                                            \
      "A list of the built in commands this shell supports:\n"                                        \
      "\n"                                                                                                            \
      "cd [-L|[-P [-e]] [-@]] [dir]            Changes the directory to whereever specified\n"                                                                                         \
      "pwd[-LP]                                Prints the absolute path of where you currently are.\n"                                                    \
      "echo[-neE] [arg ...]                    Writes arguments to the standard output\n"            \
      "set[-abefhkmnptuvxBCHP] [-o option-n..] Modify or create environmental variables\n"                                                                                                                                                                                                   \
  );                                                                                                                  \

}

/**/
void history_exec(){
  printf("History dumped: \n");
  History* ptr = head;
  if(head != NULL){
    while(ptr != NULL){
      fprintf(stderr, "%s\n", ptr->cmd);
      ptr = ptr -> next;
    }
  }
}

void clearHistory_exec(){
  char string[MAX_INPUT];
  strcpy(string, getenv("HOME"));
  char *fileName = strcat(string, "/.320sh_history");
  fclose(fopen(fileName, "w"));

  if(head != NULL){
    while(head != NULL){
      head = head -> next;
    }
  }
}

/**/
int cd_exec(char **argv){
  static char current_dir[PATH_MAX] = {'\0'};
  static char temp[PATH_MAX];
  if(argv[1] == NULL){
    strcpy(current_dir,get_current_dir());/*save current dir*/
    if(chdir("/")!= 0){
      return 1;/*error*/
    }/*end if*/
  }else{
    if(!strcmp(argv[1],"-")){
      if(current_dir[0] != '\0')/*making sure*/
        fprintf(stderr, "%s\n", current_dir);/*displays prev directory*/
      else{/*no prev one*/
        fprintf(stderr, "%s\n", "320sh: cd: OLDPWD not set");/*displays prev directory*/
        return 0;/*graceful*/
      }

      strcpy(temp,current_dir);/*save current dir before changing*/
      strcpy(current_dir,get_current_dir());/*save current dir*/
      if(chdir(temp)!= 0){
          return 1;/*error*/
        }/*end if*/
    }else{/*else is what ever*/
      strcpy(current_dir,get_current_dir());/*save current dir*/
      if(chdir(argv[1])!= 0){
          return 1;/*error*/
        }/*end if*/
      }
    }/*end else*/

    return 0;/*everything went well*/
}


int pwd_exec(void){
  char directory[PATH_MAX];
  if(getcwd(directory, sizeof(directory))!= NULL){
    fprintf(stderr, "%s\n", directory);
  }else{
    return 1;
  }
  return 0;/*everything went well*/
}

/**/
int echo_exec(char **argv, int argc){
  char *path; /*returned directory from getenv*/
  char path_name[PATH_MAX];
  if(argc == 1){
    fprintf(stderr, "\n");
    return 0;
  }

  for(int i = 1; i < argc; i++){
    if(argv[i][0] == '$'){/*Check if first char is $*/
      int j = 1;//
      if(argv[i][j] == '?'){
        fprintf(stderr, "%d ", status_child);
      }
      if(argv[i][j] != '$'){
        if(((path = getenv(++argv[i]))) != NULL){
          fprintf(stderr, "%s", path);
        }
      } else {
        while(argv[i][j] != '\0' && (argv[i][j] == '$')){
          j++;
          fprintf(stderr, "%d", getppid());
          if(argv[i][j] == '$' && argv[i][j+1] != '\0' && argv[i][j+1] != '$'){
            j++;

            int k = 1;
            path_name[0]  = argv[i][j];
          for(;(argv[i][j+1] != '\0') && (argv[i][j+1] != '$');j++, k++){
            path_name[k] = argv[i][j+1];
          }
          j++;
          path_name[k] = '\0';
            if(((path = getenv(path_name))) != NULL){
              fprintf(stderr, "%s", path);
              while(argv[i][j+1] != '\0' && argv[i][j+1] != '$')j++;
                if(argv[i][j+1] == '\0')
                  fprintf(stderr, "%c", argv[i][j]);
            }
          }
        }
        // fprintf(stderr, " ");
        fprintf(stderr, "%s ", (argv[i]+j));
      }
    }else if(argv[i][0] == '"'){
        int j = 1;
        while(argv[i][j]!='"'){
          fprintf(stderr, "%c", argv[i][j]);
          j++;
        }
        j = 0;
    }else{
      int j = 0;
      while(argv[i][j]!='$'&&argv[i][j]!='\0'){
        fprintf(stderr, "%c", argv[i][j]);
        j++;
      }
      if(argv[i][j] == '$'){/*Check if first char is $*/
        j++;
        if(argv[i][j] == '?'){
          fprintf(stderr, "%d ", status_child);
        }
        if(argv[i][j] != '$'){
          if(((path = getenv(argv[i]+j))) != NULL){
            fprintf(stderr, "%s", path);
          }
        } else {
          while(argv[i][j] != '\0' && (argv[i][j] == '$')){
            j++;
            fprintf(stderr, "%d", getppid());
            if(argv[i][j] == '$' && argv[i][j+1] != '\0' && argv[i][j+1] != '$'){
              j++;

              int k = 1;
              path_name[0]  = argv[i][j];
            for(;(argv[i][j+1] != '\0') && (argv[i][j+1] != '$');j++, k++){
              path_name[k] = argv[i][j+1];
            }
            j++;
            path_name[k] = '\0';
              if(((path = getenv(path_name))) != NULL){
                fprintf(stderr, "%s", path);
                while(argv[i][j+1] != '\0' && argv[i][j+1] != '$')j++;
                  if(argv[i][j+1] == '\0')
                    fprintf(stderr, "%c", argv[i][j]);
              }
            }
          }
          // fprintf(stderr, " ");

          fprintf(stderr, "%s ", (argv[i]+j));
        }
      }
    }/*end if 1*/
  }
  fprintf(stderr, "\n");
  return 0;
}

/**/
int set_exec(char **argv, int argc){

  if((argc >= 4) && (!strcmp(argv[2], "="))){
    if(!setenv(argv[1], argv[3], 0)){
      return 0;
    }
  }
  return 1; /*failed*/
}
/*gets current directory*/
char *get_current_dir(char *current){
  char directory[PATH_MAX];
  char *ptr;
  if((ptr = getcwd(directory, sizeof(directory))) == NULL){
    return NULL;
  }
  return ptr;/*everything went well*/
}


/*checks if file exists*/
int stat_check (char *file){
  struct stat stats;
  char *en[MAX_INPUT];
  char env[MAX_INPUT];
  strcpy(env, getenv("PATH"));

  int argc = tokenize_k(env, en);
  int i = 0;
  char name[MAX_INPUT];
  while(i < argc){
    strcpy(name, en[i]);
    strcat(name, "/");
    strcat(name, file);
    if((stat(name, &stats) == 0)||((lstat(file, &stats)) == 0)){
      // printf("%s\n", name);
      return 0;
    }
    i++;
  }
  // printf("%s %d %s\n", file,stat("/bin/ls", &stats), env);
  return 1;
}


int tokenize_k(char *buf, char **argv){
  const char* delim = {":"};
  char *ptr;
  /*remove any trailing spaces*/
  while(*buf != '\0' && (*buf == ' '))
    buf++;
  int argc = 0;
/*Removes new line before getting into the loop*/
  if((ptr = strchr(buf, '\n')) != NULL){
    *ptr = '\0';/*Make it a null instead*/
  }

while((argv[argc] = strtok(buf, delim))!=NULL){
  argc++;/*increment argc*/
  buf = buf + strlen(argv[argc-1])+1;/*set buffer to correct position*/
  /*Remove any white spaces between words*/
  while(*buf != '\0' && (*buf == ' '))
    buf++;

}
argv[argc] = NULL;
    
return argc;
}

History *tracker;

void enqueue(char *command) {
	History* temp = (History*)malloc(sizeof(History));
  History* cursor = head;
  strcpy(temp->cmd, command);
	temp->next = NULL;
  if(head == NULL){
    temp -> prev = NULL;
    head = temp;
    tracker = head;
    historyCounter++;
    return;
  }
  while(cursor -> next != NULL){
    cursor = cursor->next;
  }
  temp->prev = cursor;
  cursor->next = temp;
  tracker = temp;
  if(historyCounter == 50){
    dequeue();
  }
  // free(temp);
}

void dequeue() {
	History* temp = head;
	if(head == NULL) {
		return;
	}
  if(head -> next == NULL){
    head = NULL;
  } else {
    head = head -> next;
  }
	free(temp);
}

char cmd_temp[MAX_INPUT];
char *get_prev(){
  History *temp = tracker;
  // if(head==NULL){
  //   return "";
  // }
  if((tracker->prev == NULL) && (tracker ->next == NULL)){ /*one item in list*/
    return tracker->cmd;
  }else if(tracker->next == NULL){ /*end of list*/
    strcpy(cmd_temp, tracker->cmd);
    tracker = temp->prev;
    return cmd_temp;
  }else if(tracker->prev != NULL && tracker->next !=NULL){ /*middle*/
  strcpy(cmd_temp, tracker->cmd);
  tracker = temp->prev;
  return cmd_temp;
  }else{
    return tracker->cmd; /*beginning of list*/
  }
}

char *get_next(){
  History *temp = tracker;
  if((tracker->prev == NULL) && (tracker ->next == NULL)){ /*one item in list*/
    return "";
  }else if(tracker->next == NULL){ /*end of list*/
    return "";
  }else if(tracker->prev != NULL && tracker->next !=NULL){ /*middle*/
  strcpy(cmd_temp, tracker->next->cmd);
  tracker = temp->next;
  return cmd_temp;
  }else{
    strcpy(cmd_temp, tracker->next->cmd);
    tracker = temp->next;
    return cmd_temp; /*beginning of list*/
  }

}

// History* tail = NULL;
/*Assume no input line will be longer than 1024 bytes */
/*#define MAX_INPUT 1024 , already defined in csapp*/
void write_history(){
    char string[MAX_INPUT];
    strcpy(string, getenv("HOME"));
    char *fileName = strcat(string, "/.320sh_history");
    // printf("Writing to: %s\n", fileName);
    unlink(fileName);
    //char *fileName = "testfile";
    FILE *file = fopen(fileName, "w+");
    if(head != NULL){
      History* cursor = head;
      if(cursor -> prev == NULL && cursor -> next == NULL){ /*one item in list*/
        fputs(cursor->cmd, file);
      } else {
        while(cursor -> next != NULL){
          fprintf(file, "%s", cursor->cmd);
          fprintf(file, "\n");
          cursor = cursor -> next;
        }
      }
    }
    fclose(file);
}

void read_history(){
    char string[MAX_INPUT];
    strcpy(string, getenv("HOME"));
    char *fileName = strcat(string, "/.320sh_history");
    // printf("Reading from: %s\n", fileName);
    //char *fileName = "testfile";
    FILE* file = fopen(fileName, "r");
    char input[MAX_INPUT];
    struct stat st;
    // printf("size %ld\n", st.st_size);
    if(stat(fileName, &st) == 0){
      while(fgets(input, MAX_INPUT, (FILE *)file)){
        size_t length = strlen(input);
        if(input[length-1] == '\n'){
          input[--length] = '\0';
        }
        enqueue(input);
      }

      fclose(file);
    }else{
      fprintf(stderr,"%s\n", "file not found");
    }
}