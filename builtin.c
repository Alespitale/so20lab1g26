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
    if (strcmp(scommand_front(cmd),"cd") == 0){
        scommand_pop_front(cmd);
        int ret = chdir(scommand_front(cmd));
        
        if(ret != 0){
            switch (ret)
            {
            case EFAULT: 
                perror("path apunta fuera de su espacio de direcciones accesible.");
                break;
            case ENAMETOOLONG:
                perror("path es demasiado largo.");
                break;
            case ENOENT:
                perror("El fichero no existe.");
                break;
            case ENOMEM:
                perror("No hay suficiente memoria disponible en el núcleo.");
                break;
            case ENOTDIR:
                perror("Un componente del camino path no es un directorio.");
                break;
            case EACCES:
                perror("Ha sido denegado el permiso de búsqueda en uno de los componentes del camino path.");
                break;
            case ELOOP:
                perror("Se han encontrado demasiados enlaces simbólicos al resolver path");
                break;
            case EIO:
                perror("Ha ocurrido un error de E/S.");
                break;
            default:
                perror("No se pudo cambiar de directorio");
                break;
            }
            cmd = scommand_destroy(cmd);
            exit(0);
        }
        
    }else {
        scommand_pop_front(cmd);
        cmd = scommand_destroy(cmd);
        exit(0);
    }
} 


