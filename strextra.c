#include "strextra.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <gmodule.h>

char * strmerge(char *s1, char *s2){
  assert(s1 != NULL &&  s2 != NULL);
  gsize size = (strlen(s1) + strlen(s2) + 1);
  s1 = realloc(s1,size);
  assert(s1 != NULL);
  strcat(s1,s2);
  assert((s1 != NULL) && strlen(s1) == size-1);
  return s1;
}
