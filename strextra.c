#include "strextra.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <gmodule.h>

char * strmerge(char *s1, char *s2){
  assert(s1 != NULL &&  s2 != NULL);
  char * merge = calloc(strlen(s1) + strlen(s2) + 1, sizeof(char));
  assert(merge != NULL);
  strcpy(merge, s1);
  strcat(merge, s2);
  assert((merge != NULL) && strlen(merge) == strlen(s1) + strlen(s2));
  return (merge);
}
