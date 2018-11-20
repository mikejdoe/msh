#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <wait.h>

int bg_flag = 0;

void loop();
char **parse_cmd(char* line);


void loop(){
    char **cmd;
    char *line;
    pid_t child;
    int status;

    for(;;){
        waitpid(-1, 0, WNOHANG);
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
            if(!strcmp(line, "y"))
                return;
        }

        child = fork();
        if(child == 0){
            execvp(cmd[0], cmd);
        } else {
            if(bg_flag){
                printf("%i \n", child);
            }
            else
                waitpid(child, &status, WUNTRACED);
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