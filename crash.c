#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "command.h"
#include "parser.h"
#include "execute.h"
#include "builtin.h"
#include <limits.h>

int main(void){ 
    printf("\n                               ########################## Crash by Tres Acordes ########################## \n\n");
    char hostname[1000];
    char direc[1000];
    char * username = getenv("USER");
    gethostname(hostname,sizeof(hostname));
    getcwd(direc,sizeof(direc));
    Parser parser = parser_new(stdin); 
    if(parser == NULL){
        perror("Fallo el Parser");
        exit(EXIT_FAILURE);
    }
    pipeline pipe = pipeline_new();
    while(!parser_at_eof(parser)){
        printf("%s@%s:~%s-> " , username,hostname,direc);
        pipe = parse_pipeline(parser);
        if(pipe != NULL){
        execute_pipeline(pipe);
        }
    }
    pipeline_destroy(pipe);
    parser_destroy(parser);
return 0;
}