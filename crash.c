#include "command.h"    //cabecera
#include "parser.h"     // cabecera
#include "execute.h"    // cabecera
#include "builtin.h"   // cabecera
#include <stdlib.h>    //libreria estandar
#include <stdio.h>     // input/output
#include <unistd.h>     //--
#include <sys/types.h>  //wait
#include <sys/wait.h>   // wait
#include <limits.h> // para user, hostname


int main(void){ 
    printf("\n                               ########################## Crash by Tres Acordes ########################## \n\n");
    char hostname[1000];
    char direc[1000];
    char * username = getenv("USER");
    gethostname(hostname,sizeof(hostname));
    getcwd(direc,sizeof(direc));
    Parser parser = parser_new(stdin); 
    if(parser == NULL){
        perror("Fallo el Parser\n");
        exit(EXIT_FAILURE);
    }
    while(!parser_at_eof(parser)){
        printf("%s@%s:~%s-> " , username,hostname,direc);
        pipeline pipe = parse_pipeline(parser);
        while(waitpid(-1,0,WNOHANG)>0);
        if(pipe != NULL){
        execute_pipeline(pipe);
        pipeline_destroy(pipe);
        }
    }
    return 0;
}
