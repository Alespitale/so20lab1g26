#include "command.h"
#include "strextra.h"
#include <gmodule.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

struct scommand_s {
	GSList * str;
	gchar * in;
	gchar * out;
};

struct pipeline_s {
	GSList * sc_pipe;
	bool wait;
};

scommand scommand_new(void){
	scommand result = calloc(1, sizeof(struct scommand_s));
	result->str = NULL;
	result->in = NULL;
	result->out = NULL;
	assert(result != NULL && scommand_is_empty(result) &&	scommand_get_redir_in(result) == NULL &&
 	scommand_get_redir_out(result) == NULL);
	return result;
}

scommand scommand_destroy(scommand self){	
	assert(self != NULL);
	g_slist_free(self->str);
	if(self->in != NULL){
		free(self->in);
		self->in = NULL;
	}
	if(self->out != NULL){
		free(self->out);
		self->out = NULL;
	}
	free(self);
	self = NULL;
	assert(self == NULL);
	return (self);
}

void scommand_push_back(scommand self, char * argument){
	assert(self != NULL && argument != NULL);
	self->str = g_slist_append(self->str, strdup(argument));
	free(argument);
	argument = NULL;
	assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self){
	assert(self!=NULL && !scommand_is_empty(self));
	self->str = g_slist_remove(self->str,g_slist_nth_data(self->str,0));
}

void scommand_set_redir_in(scommand self, char * filename){
	assert(self != NULL);
	self->in = filename;
}

void scommand_set_redir_out(scommand self, char * filename){
	assert(self != NULL);
	self->out = filename;
}

bool scommand_is_empty(const scommand self){
	assert(self != NULL);
	bool b = (self->str == NULL);
	return b;
}

unsigned int scommand_length(const scommand self){
	assert(self != NULL);
	guint n = g_slist_length(self->str);
	assert((n==0) == scommand_is_empty(self));
	return n;
}

char * scommand_front(const scommand self){
	assert(self != NULL && !scommand_is_empty(self));
	char * result = strdup(g_slist_nth_data(self->str,0));
	assert(result != NULL);
	return result;
}

char * scommand_get_redir_in(const scommand self){
	assert(self != NULL);
	return (self->in);
}

char * scommand_get_redir_out(const scommand self){
	assert(self != NULL);
	return (self->out);
}

char * scommand_to_string(const scommand self){
	assert(self != NULL);
	guint len = scommand_length(self);
	gchar * result = NULL;
	if (!scommand_is_empty(self)){
		gchar * p = g_slist_nth_data(self->str,1);
		result = strmerge(scommand_front(self),p);
		for (guint i = 2 ; i < len ; i++){
			p = g_slist_nth_data(self->str,i);
			result = strmerge(result,p);
		}
		if (scommand_get_redir_out(self) != NULL){
			gchar * salida = " > ";
			result = strmerge(result, salida);
			result = strmerge(result, self->out);
		}
		if (scommand_get_redir_in(self) != NULL){
			gchar * entrada = " < ";
			result = strmerge(result, entrada);
			result = strmerge(result, self->in);
		}
	}
	assert(scommand_is_empty(self) || scommand_get_redir_in(self)==NULL || scommand_get_redir_out(self)==NULL || strlen(result)>0);
	return result;
}


/*---------------------------Pipeline---------------------------------------*/

pipeline pipeline_new(void){
	pipeline pipe = calloc(1,sizeof(struct pipeline_s));
	pipe->sc_pipe = NULL;
	pipe->wait = true;
	assert(pipe != NULL && pipeline_is_empty(pipe) && pipeline_get_wait(pipe));
	return pipe;
}

pipeline pipeline_destroy(pipeline self){
	assert(self != NULL);
	g_slist_free(self->sc_pipe);
	free(self);
	self = NULL;
	return self;
}

void pipeline_push_back(pipeline self, scommand sc){
	assert(self != NULL && sc != NULL);
	self->sc_pipe = g_slist_append(self->sc_pipe, sc);
	sc = scommand_destroy(sc);
	sc = NULL;
	assert(!pipeline_is_empty(self));
}

void pipeline_pop_front(pipeline self){
	assert(self != NULL && !pipeline_is_empty(self));
	self->sc_pipe = g_slist_remove(self->sc_pipe,self->sc_pipe->data);
}

void pipeline_set_wait(pipeline self, const bool w){
	assert(self != NULL);
	self->wait = w;
}

bool pipeline_is_empty(const pipeline self){
	assert(self != NULL);
	return (g_slist_length(self->sc_pipe) == 0);
}

unsigned int pipeline_length(const pipeline self){
	assert(self != NULL);
	guint n = g_slist_length(self->sc_pipe);
	assert((pipeline_length(self)==0) == pipeline_is_empty(self));
	return n;
}

scommand pipeline_front(const pipeline self){
	assert(self!=NULL && !pipeline_is_empty(self));
	scommand sc = self->sc_pipe->data;
	assert(sc != NULL);
	return sc;
}

bool pipeline_get_wait(const pipeline self){
	assert(self!=NULL);
	return (self->wait == true);
}

char * pipeline_to_string(const pipeline self){
	assert(self!=NULL);
	gchar * c = " | ";
	gchar * merge = NULL;
	guint len = pipeline_length(self);
	scommand q = g_slist_nth_data(self->sc_pipe,0);
	if(len == 1){
		merge = scommand_to_string(q);
	}else{
		for (guint i = 0 ; i < len-1 ; i++){
			q = g_slist_nth_data(self->sc_pipe,i);
			merge = scommand_to_string(q);
			merge = strmerge(merge,c);
		}
		q = g_slist_nth_data(self->sc_pipe,len);
		merge = scommand_to_string(q);
	}
	assert(pipeline_is_empty(self) || pipeline_get_wait(self) || strlen(merge)>0);
	return merge;
}
