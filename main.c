#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <wait.h>

int bg_flag = 0;
pid_t current_child = 0;

void loop();
char **parse_cmd(char* line);
void sigint();
void sigtstp();
void sig_chld(int signo);

void sigint(){
    kill(current_child, SIGINT);
}

void sigtstp(){
    kill(current_child, SIGTSTP);
}

void sig_chld(int signo) {
    pid_t   pid;
    int     stat;
    while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        printf("child %d terminated\n", pid);
        current_child = 0;
    }
}

void loop(){
    char **cmd;
    char *line;
    pid_t child;
    int status;

    for(;;){
        line = readline("msh$ ");
        if(line[strlen(line) -1] == '&') {
            bg_flag = 1;
            line[strlen(line) -1] = '\0';
        }
        else
            bg_flag = 0;

        cmd = parse_cmd(line);
        if(!strcmp(line, ""))
            continue;
        if(!strcmp(cmd[0], "logout")) {
            line = readline("really? [y/N] ");
            if(!strcmp(line, "y")) {
                if(current_child != 0){
                    printf("unfinished jobs: %i", current_child);
                    continue;
                }
                return;

            }
        }

        if(!strcmp(cmd[0], "fg")){
            kill(current_child, SIGCONT);
        }
        if(!strcmp(cmd[0], "bg")){
            bg_flag = 1;
        }


        child = fork();
        if(child == 0){
            setpgid(0,0);
            execvp(cmd[0], cmd);
        } else {
            current_child = child;
            signal(SIGINT, sigint);
            signal(SIGTSTP, sigtstp);
            signal(SIGCHLD, sig_chld);

            if(bg_flag){
                printf("%i \n", child);
            }
            else{
                waitpid(child, &status, WUNTRACED);
            }
        }

        free(cmd);
        free(line);
    }
}

char **parse_cmd(char* line){
    char **cmd = malloc(10 * sizeof(char*));
    char *token;

    token = strtok(line, " ");
    int i;
    for(i = 0; token != NULL; i++){
        cmd[i] = token;
        token = strtok(NULL, " ");
    }
    cmd[i] = NULL;
    return cmd;
}


int main() {
    loop();
    return 0;
}