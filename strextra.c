#include "strextra.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <gmodule.h>

char * strmerge(char *s1, char *s2){
  assert(s1 != NULL &&  s2 != NULL);
  GString * merge = g_string_new(s1);
  merge = g_string_append(merge,s2);
  assert((merge != NULL) && strlen(merge->str) == strlen(s1) + strlen(s2));
  return (merge->str);
}
