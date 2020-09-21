#include "builtin.h"
#include "strextra.h"
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "tests/syscall_mock.h"

bool builtin_is_internal(scommand cmd){
    assert(cmd != NULL);
    int a = strcmp(scommand_front(cmd),"cd");
    int b = strcmp(scommand_front(cmd),"exit");
    return (a == 0 || b == 0);
}

void builtin_exec(scommand cmd){
    assert(builtin_is_internal(cmd));
    if (strcmp(scommand_front(cmd),"cd") == 0){     // El comando es cd
        scommand_pop_front(cmd);
        chdir(scommand_front(cmd));
        cmd = scommand_destroy(cmd);
    }else {                                         // El comando es exit
        cmd = scommand_destroy(cmd);
        exit(0);
    }
    
} 


