#include "execute.h"
#include "builtin.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <gmodule.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>



void execute_pipeline(pipeline apipe){
    unsigned int N = pipeline_length(apipe);
    if(N == 1){                             // Es un comando simple
        if(builtin_is_internal(pipeline_front(apipe))){
            builtin_exec(pipeline_front(apipe));
            apipe = pipeline_destroy(apipe);
        }else{
            pid_t pid = fork();
            if (pid < 0){                   // falló fork; exit 
                fprintf(stderr, "fork failed\n");
                exit(1);
            }else if (pid == 0){            // proceso hijo (nuevo proceso)
                scommand sc = pipeline_front(apipe);
                guint L = scommand_length(sc);               
                //char * arg = calloc(L+1,sizeof(char));
                char * arg[L];
                char * dir_in = scommand_get_redir_in(sc);
                char * dir_out = scommand_get_redir_out(sc);
                if(dir_in != NULL){
                    close (0);
                    open(dir_in,O_CREAT|O_WRONLY|O_TRUNC);
                }
                for (guint i = 0; i < L; i++){
                    arg[i] = scommand_front(sc);
                    scommand_pop_front(sc);
                }
                sc = scommand_destroy(sc);
                arg[L] = NULL;
                execvp(arg[0], arg);  
                exit(1);                     // asegura que no sigan procesos 
            }else{                           // (pid > 0) proceso padre
                if(pipeline_get_wait(apipe)){
                    wait(NULL);
                }
            }
        }
    }else{                                  // Hay mas de un comando
        for (guint i =0; i < N; i++){
            int pipes[2];                   //uno para el extremo de escritura y otro para el extremo de lectura
            pipe (pipes);
            pid_t pid = fork();
            scommand sc = pipeline_front(apipe);
            if(pid < 0){                    //error

            }
            else if(pid == 0){              // hijo

            }
            else{                           //padre

            } 
        }
    }
}


