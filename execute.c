#include "execute.h"
#include "builtin.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <gmodule.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include "tests/syscall_mock.h"

static char ** scommand_adapted(scommand self){
    char **argv = calloc(scommand_length(self),sizeof(char *));
    for (guint i = 0 ; i < scommand_length(self) ; i++){
        argv[i] = strdup(scommand_front(self));
        scommand_pop_front(self);
    }
    return argv;
}


static void check_fd_out(int fd){
    if (fd == -1){
        perror("fallo mock_open");
        exit(0);
    }
    if(dup2(fd,1) == -1){
        perror("fallo dup");
        exit(0);
    }
    if(close(fd) == -1){
        perror("fallo close");
        exit(0);
    }
}
static void check_fd_in(int fd){
    if (fd == -1){
        perror("fallo mock_open");
        exit(0);
    }
    if(dup2(fd,0) == -1){
        perror("fallo dup");
        exit(0);
    }
    if(close(fd) == -1){
        perror("fallo close");
        exit(0);
    }
}

void execute_pipeline(pipeline apipe){
    assert(!pipeline_is_empty(apipe));
    unsigned int N = pipeline_length(apipe);
    if(N == 1){             //Es un comando simple
        if(builtin_is_internal(pipeline_front(apipe))){
            builtin_exec(pipeline_front(apipe));
            //apipe = pipeline_destroy(apipe);
        }
    }else{
            char ** cmd_adapted = scommand_adapted(pipeline_front(apipe));
           // for(guint i = 0; i < cmd_adapted[i]; i++){
           //     strdup(cmd_adapted[i]);
           // }
            pid_t pid = fork();
            if (pid < 0){           // fork failed; exit
                fprintf(stderr, "fork failed\n");
                exit(1);
            }else if ( pid > 0){    // padre
                if (pipeline_get_wait(apipe)){
                    wait(NULL);
                }else{
                    //for (guint i=0 ; i < cmd_adapted[i]; i++){
                    //    g_free(cmd_adapted[i]);
                   //}
                    //g_free(cmd_adapted);
                }
            }else {   // hijo
                if (scommand_get_redir_in(pipeline_front(apipe)) == NULL && scommand_get_redir_out(pipeline_front(apipe)) == NULL){
                    execvp(cmd_adapted[0],cmd_adapted);
                }else if (scommand_get_redir_in(pipeline_front(apipe)) == NULL && scommand_get_redir_out(pipeline_front(apipe)) != NULL){
                    int fd = open(scommand_get_redir_out(pipeline_front(apipe)),0,O_CREAT|O_WRONLY|O_TRUNC);
                    check_fd_out(fd);
                    execvp(cmd_adapted[0],cmd_adapted);
                }else if(scommand_get_redir_in(pipeline_front(apipe)) != NULL && scommand_get_redir_out(pipeline_front(apipe)) == NULL){
                    int fd = open(scommand_get_redir_in(pipeline_front(apipe)),0,O_CREAT|O_WRONLY|O_TRUNC);
                    check_fd_in(fd);
                    execvp(cmd_adapted[0],cmd_adapted);
                }else{
                    int fd1 = open(scommand_get_redir_in(pipeline_front(apipe)),0,O_CREAT|O_WRONLY|O_TRUNC);
                    check_fd_in(fd1);
                    int fd = open(scommand_get_redir_out(pipeline_front(apipe)),0,O_CREAT|O_WRONLY|O_TRUNC);
                    check_fd_out(fd);
                    execvp(cmd_adapted[0],cmd_adapted);                    
                }
            }
    }
}





/*
char ** scommand_adapted(scommand self){
    char **argv = calloc(scommand_length(self),sizeof(char *));
    for (guint i = 0 ; i < scommand_length(self) ; i++){
        argv[i] = strdup(scommand_front(self));
        scommand_pop_front(self);
    }
    return argv;
}

void execute_pipeline(pipeline apipe){
    assert(apipe != NULL);
    unsigned int N = pipeline_length(apipe);
    if(N == 1){             //Es un comando simple
        if(builtin_is_internal(pipeline_front(apipe))){
            builtin_exec(pipeline_front(apipe));
            //apipe = pipeline_destroy(apipe);
        }else{
            pid_t pid = fork();
            if (pid < 0){           // fork failed; exit
                fprintf(stderr, "fork failed\n");
                exit(1);
            }else if ( pid > 0){    // padre
                if (pipeline_get_wait(apipe)){
                    wait(NULL);
                   // for (guint i=0 ; i < cmd_adapted[i]; i++){
                   //    g_free(cmd_adapted[i]);
                   // }
                   // g_free(cmd_adapted);
                }
            }else if ( pid == 0){   // hijo
                char ** cmd_adapted = scommand_adapted(pipeline_front(apipe));
               // for(guint i = 0; i < cmd_adapted[i]; i++){
               //     strdup(cmd_adapted[i]);
              //  }
                if (scommand_get_redir_in(pipeline_front(apipe)) == NULL && scommand_get_redir_out(pipeline_front(apipe)) == NULL){
                    execvp(cmd_adapted[0],cmd_adapted);
                }else if (scommand_get_redir_in(pipeline_front(apipe)) == NULL && scommand_get_redir_out(pipeline_front(apipe)) != NULL){
                    int fd = mock_open(scommand_get_redir_in(pipeline_front(apipe)),O_CREAT|O_WRONLY|O_TRUNC);
                    if (fd == -1){
                        perror("fallo mock_open");
                        exit(2);
                    }
                    if(dup2(fd,0) == -1){
                        perror("fallo dup");
                        exit(2);
                    }
                    if(close(fd)== -1){
                        perror("fallo close");
                    }
                    int fd1 = mock_open(scommand_get_redir_in(pipeline_front(apipe)),O_CREAT|O_WRONLY|O_TRUNC);
                    if (fd1 == -1){
                        perror("fallo mock_open");
                        exit(2);
                    }
                    if(dup2(fd1,1) == -1){
                        perror("fallo dup");
                    }
                    if(close(fd1)== -1){
                        perror("fallo close");
                    }
                    execvp(cmd_adapted[0],cmd_adapted);
                }
            }
        }
    }
}
*/
