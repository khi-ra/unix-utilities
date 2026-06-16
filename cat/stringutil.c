#include "stringutil.h"
#include <stdlib.h>
#include <string.h>

/* Write ERROR_MESSAGE into ERROR_BUFFER. */
void write_error(char *error_message, char **error_buffer)
{
  copy_string(error_message, error_buffer, strlen(error_message));
}

/* Copy contents of IN to dynamic string OUT */
void copy_string(char *in, char **out, size_t in_size)
{
  *out = realloc(*out, in_size + 1);
  memmove(*out, in, in_size);
  *(*out + in_size) = 0;
}
