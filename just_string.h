/*
 * Just string things
 */

#ifndef JUST_STRING_H
#define JUST_STRING_H

typedef struct string_s* string;

/* Add len amount of data to string */
int append_len_string(string s, const char *data, size_t data_len);

/* Add data to end of string, data must be null terminated */
int append_string(string s, const char *data);

/* change the allocated memory for string */
int resize_string(string s, size_t new_size);

const char* get_string(string s);

/* initialize string */
string new_string();

/* Free allocation of string data */
void free_string(string s);


#endif /* JUST_STRING_H */