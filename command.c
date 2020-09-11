#include "command.h"
#include "strextra.h"
#include <gmodule.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

struct scommand_s {
	GQueue * str;
	char * in;
	char * out;
};

scommand scommand_new(void){
	scommand result = calloc(1, sizeof(struct scommand_s));
	assert(result != NULL);
	result->str = g_queue_new();
	result->in = NULL;
	result->out = NULL;
	assert(result != NULL && scommand_is_empty(result) && scommand_get_redir_in(result) == NULL &&
 	scommand_get_redir_out(result) == NULL);
	return result;
}

scommand scommand_destroy(scommand self){	
	assert(self != NULL);
	g_queue_free_full(self->str, g_free);
	if(self->in != NULL){
		g_free(self->in);
		self->in = NULL;
	}
	if(self->out != NULL){
		g_free(self->out);
		self->out = NULL;
	}
	free(self);
	self = NULL;
	assert(self == NULL);
	return (self);
}

void scommand_push_back(scommand self, char * argument){
	assert(self != NULL && argument != NULL);
	g_queue_push_tail(self->str,argument);
}

void scommand_pop_front(scommand self){
	assert(self!=NULL && !scommand_is_empty(self));
    gpointer head = g_queue_pop_head(self->str);
    g_free(head);
}

void scommand_set_redir_in(scommand self, char * filename){
	assert(self != NULL);
	g_free(self->in);
	self->in = filename;
}

void scommand_set_redir_out(scommand self, char * filename){
	assert(self != NULL);
	g_free(self->out);
	self->out = filename;
}

bool scommand_is_empty(const scommand self){
	assert(self != NULL);
	return (g_queue_is_empty(self->str));
}

unsigned int scommand_length(const scommand self){
	assert(self != NULL);
	guint n = g_queue_get_length(self->str);	assert((n==0) == scommand_is_empty(self));
	return n;
}

char * scommand_front(const scommand self){
	assert(self != NULL && !scommand_is_empty(self));
	char * result = g_queue_peek_head(self->str);
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
	assert(self!=NULL);
	guint len_list = g_queue_get_length(self->str);
	gchar * string = strdup("");
	for(unsigned int i = 0 ; i < len_list; i++){
		string = strmerge(string, g_queue_peek_nth(self->str,i));
		string = strmerge(string, " ");
	}
	if(scommand_get_redir_in(self) != NULL){
		string = strmerge(string, "<"); 
		string = strmerge(string, " ");
		string = strmerge(string, self->in);
		string = strmerge(string, " ");
	}
	if(scommand_get_redir_out(self) != NULL){
		string = strmerge(string, ">");
		string = strmerge(string, " ");
		string = strmerge(string, self->out);
	}
	assert(scommand_is_empty(self) || scommand_get_redir_in(self)==NULL || scommand_get_redir_out(self)==NULL || strlen(string)>0);
	return string;
}

/*---------------------------Pipeline---------------------------------------*/

struct pipeline_s {
	GQueue * sc_pipe;
	bool wait;
};

pipeline pipeline_new(void){
	pipeline pipe = calloc(1,sizeof(struct pipeline_s));
	pipe->sc_pipe = g_queue_new();
	pipe->wait = true;
	assert(pipe != NULL && pipeline_is_empty(pipe) && pipeline_get_wait(pipe));
	return pipe;
}

static void destroy_scmd(gpointer scmd){
	scommand_destroy(scmd);
}

pipeline pipeline_destroy(pipeline self){
	assert(self != NULL);
	g_queue_free_full(self->sc_pipe, &destroy_scmd);
	free(self);
	self = NULL;
	return self;
}

void pipeline_push_back(pipeline self, scommand sc){
	assert(self != NULL && sc != NULL);
	g_queue_push_tail(self->sc_pipe,sc);
	assert(!pipeline_is_empty(self));
}

void pipeline_pop_front(pipeline self){
	assert(self != NULL && !pipeline_is_empty(self));
    gpointer head = g_queue_pop_head(self->sc_pipe);
    head = scommand_destroy(head);
    head = NULL;
}

void pipeline_set_wait(pipeline self, const bool w){
	assert(self != NULL);
	self->wait = w;
}

bool pipeline_is_empty(const pipeline self){
	assert(self != NULL);
	bool b;
	return (b = g_queue_is_empty(self->sc_pipe));
}

unsigned int pipeline_length(const pipeline self){
	assert(self != NULL);
	guint n = g_queue_get_length(self->sc_pipe);
	assert((n==0) == pipeline_is_empty(self));
	return n;
}

scommand pipeline_front(const pipeline self){
	assert(self!=NULL && !pipeline_is_empty(self));
	scommand sc = g_queue_peek_head(self->sc_pipe);
	assert(sc != NULL);
	return sc;
}

bool pipeline_get_wait(const pipeline self){
	assert(self!=NULL);
	return (self->wait == true);
}

char * pipeline_to_string(const pipeline self){
	assert(self!=NULL);
	gchar * merge = strdup("");
	gchar * cmd = NULL;
	guint len = pipeline_length(self);
	for (guint i = 0 ; i < len ; i++){
		cmd = scommand_to_string(g_queue_peek_nth(self->sc_pipe,i));
		merge = strmerge(merge,cmd);
		g_free(cmd);
		if(i != (len-1)){
			merge = strmerge(merge," | ");
		}else{
			merge = strmerge(merge," &");
		}
	}
	assert(pipeline_is_empty(self) || pipeline_get_wait(self) || strlen(merge)>0);
	return merge;
}
