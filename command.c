#include "command.h"

#include <gmodule.h>
#include<assert.h>
#include <stdlib.h>


struct scommand_s {
	GSList * str;
	char * in;
	char * out;
};

scommand scommand_new(void){
		scommand sc = calloc(1, sizeof(struct scommand_s));
		sc->str = g_slist_alloc();
		sc->in = NULL;
		sc->out = NULL;
	return sc;
}

scommand scommand_destroy(scommand self){	
	assert(self != NULL);
	g_slist_free(self -> str);
	free(self->in);
	self->in = NULL;
	free(self->out);
	self->out = NULL;
	free(self);
	self = NULL;
	return self;
}

void change_type(char * str){

}

void scommand_push_back(scommand self, char * argument){
	assert(self != NULL && argument != NULL);
	self->str = g_slist_append(self->str, argument);
	free(argument);
	argument = NULL;
	assert(!scommand_is_empty);
}

void scommand_pop_front(scommand self){
	assert(self!=NULL && !scommand_is_empty(self));
	self->str = g_slist_remove(self->str,self->str->data);

}

void scommand_set_redir_in(scommand self, char * filename){
	assert(self!=NULL);
	self->in = filename;

}

void scommand_set_redir_out(scommand self, char * filename){
	assert(self!=NULL);
	self -> out =filename;
}

bool scommand_is_empty(const scommand self){
	return (g_slist_length(self->str) == 0);
}

unsigned int scommand_length(const scommand self){
	unsigned int n;
	return (n = g_slist_length(self->str));
}
char * scommand_front(const scommand self){
	assert(self!=NULL && !scommand_is_empty(self));
	return self->str->data;
}
char * scommand_get_redir_in(const scommand self){
	return self ->in;
}
char * scommand_get_redir_out(const scommand self){
	return self->out;
}
char * scommand_to_string(const scommand self){
	assert(self!=NULL);
	self->str = scommand_push_back(self,self->out);
	self->str = scommand_push_back(self,self->in);
	free(self->out);
	self->out = NULL;
	free(self->in) = NULL;
	assert(scommand_is_empty(self) || scommand_get_redir_in(self)==NULL || scommand_get_rendir_out(self)==NULL || strlen(self->str)>0);
	return self->str;
}









