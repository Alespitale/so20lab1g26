#include <string.h>
#include "builtin.h"
#include <assert.h>
#include <unistd.h>

bool builtin_is_internal(scommand cmd){
    assert(cmd != NULL);
    int a = strcmp(scommand_front,"cd");
    int b = strcmp(scommand_front,"exit");
    return (a == 0 || b == 0);
}

void builtin_exec(scommand cmd){
    assert(builtin_is_internal(cmd));
    if (strcmp(scommand_front,"cd") == 0){
        scommand_pop_front(cmd);
        if(chdir(scommand_front(cmd)) != 0){
            printf("chdir to %s failed \n",scommand_front(cmd));
        }
    }else {
        scommand_pop_front(cmd);
        cmd = scommand_destroy(cmd);
        exit(0);
    }
} 
