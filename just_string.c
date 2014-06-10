#include <malloc.h>
#include <string.h>
#include <assert.h>
#include "just_string.h"

struct string_s {
	size_t size;
	char *data;
};

const char* get_string(string s)
{
	assert (s != NULL);
	return s->data;
}

int append_len_string(string s, const char *data, size_t data_len)
{
	assert(s != NULL);

	size_t s_len = strlen(s->data);

	if(resize_string(s, data_len + s_len + 1) != 0) {
		return -1;
	}

	memcpy(s->data + s_len, data, data_len);

	s->data[s_len + data_len] = 0;

	return 0;
}

int append_string(string s, const char *data)
{
	assert(s != NULL);

	return append_len_string(s, data, strlen(data));
}

int resize_string(string s, size_t new_size)
{
	assert(s != NULL);

	char *more_mem;

	more_mem = realloc(s->data, new_size);
	if(more_mem == NULL) {
		// Terrible things have happened
		free_string(s);
		return -1;
	}

	s->data = more_mem;
	s->size = new_size;

	return 0;
}

string new_string()
{
	string s;
	s = malloc(sizeof(*s));
	if(s == NULL) {
		free(s);
		return NULL;
	}

	s->data = malloc(1);
	if(s->data == NULL) {
		free(s->data);
		free(s);
		return NULL;
	}
	memset(s->data, 0, 1);
	s->size = 0;
	return s;
}

void free_string(string s)
{
	free(s->data);
	free(s);
}