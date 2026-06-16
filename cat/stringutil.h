#ifndef CUSTOMSTRING_H
#define CUSTOMSTRING_H

#include <unistd.h>

/* Copy contents of IN to dynamic string OUT */
void copy_string(char *in, char **out, size_t in_size);

/* Write ERROR_MESSAGE into ERROR_BUFFER. */
void write_error(char *error_message, char **error_buffer);

#endif
