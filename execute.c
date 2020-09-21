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
    char **argv = calloc(scommand_length(self)+1,sizeof(char *));
    guint len = scommand_length(self);
    for (guint i = 0 ; i < len ; i++){
        argv[i] = strdup(scommand_front(self));
        scommand_pop_front(self);
    }
        argv[(len)]= NULL;
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
    assert(apipe != NULL);
    unsigned int N = pipeline_length(apipe);
    if(N == 0){             //es comando vacio
    }
    if(N == 1){             //Es un comando simple
        if(builtin_is_internal(pipeline_front(apipe))){            // Es comando interno
            builtin_exec(pipeline_front(apipe));
            //apipe = pipeline_destroy(apipe);
        }
        else{                                                      // Es comando externo
            char ** cmd_adapted = scommand_adapted(pipeline_front(apipe));
            pid_t pid = fork();
            if (pid < 0){           // fork failed; exit
                fprintf(stderr, "fork failed\n");
                exit(1);
            }else if ( pid > 0){    // padre
                if (pipeline_get_wait(apipe)){
                    wait(NULL);
                }else{
                        // esto da leak
                }
            }else{     // hijo
                if (scommand_get_redir_in(pipeline_front(apipe)) == NULL && scommand_get_redir_out(pipeline_front(apipe)) == NULL){
                    execvp(cmd_adapted[0],cmd_adapted);
                }else if (scommand_get_redir_in(pipeline_front(apipe)) == NULL && scommand_get_redir_out(pipeline_front(apipe)) != NULL){
                    int fd = open(scommand_get_redir_out(pipeline_front(apipe)),O_CREAT|O_WRONLY,0);
                    check_fd_out(fd);
                    execvp(cmd_adapted[0],cmd_adapted);
                }else if(scommand_get_redir_in(pipeline_front(apipe)) != NULL && scommand_get_redir_out(pipeline_front(apipe)) == NULL){
                    int fd = open(scommand_get_redir_in(pipeline_front(apipe)),O_CREAT|O_RDONLY,S_IRUSR);
                    check_fd_in(fd);
                    execvp(cmd_adapted[0],cmd_adapted);
                }else{
                    int fd1 = open(scommand_get_redir_in(pipeline_front(apipe)),O_CREAT|O_RDONLY ,S_IRUSR);
                    check_fd_in(fd1);
                    int fd = open(scommand_get_redir_out(pipeline_front(apipe)),O_CREAT|O_WRONLY,S_IWUSR);
                    check_fd_out(fd);
                    execvp(cmd_adapted[0],cmd_adapted);                    
                }
            } 
        }
    }
}   
/*
    if(N >=1){             //Es un comando simple
            for (guint i= 0; i<N; i++){
                if(builtin_is_internal(pipeline_front(apipe))){            // Es comando interno
                builtin_exec(pipeline_front(apipe));
                }else{
                    pipe()
                }
                //apipe = pipeline_destroy(apipe);
            }
            }
        
            else{                                                      // Es comando externo
                char ** cmd_adapted = scommand_adapted(pipeline_front(apipe));
                pid_t pid = fork();
                if (pid < 0){           // fork failed; exit
                    fprintf(stderr, "fork failed\n");
                    exit(1);
                }else if ( pid > 0){    // padre
                    if (pipeline_get_wait(apipe)){
                        wait(NULL);
                    }else{
                            // esto da leak
                    }
                }else {     // hijo
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
}*/ 
