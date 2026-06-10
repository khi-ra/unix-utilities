#include "file.h"
#include "customstring.h"
#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

/* Open FILE in read-only mode and return it's file descriptor, or -1 for
   error. */
int open_file(file_struct *file, char **error_buffer)
{
  int dir_fd = open("./", O_RDONLY);

  // if path is absolute, 'dir_fd' is ignored and only the path is used
  if (dir_fd == -1 || (file->fd = openat(dir_fd, file->path, O_RDONLY)) == -1)
  {
    write_error("File cannot be opened", error_buffer);
  }

  close(dir_fd);
  return file->fd;
}

int read_file(file_struct *file, char **error_buffer)
{
  char content_buff[MAXFILEDATA];
  int nbytes;

  if ((nbytes = read(file->fd, content_buff, MAXFILEDATA)) == -1)
  {
    write_error("File cannot be read", error_buffer);
    return nbytes;
  }

  copy_string(content_buff, &file->content, nbytes);
  file->content[nbytes] = 0;

  return nbytes;
}
