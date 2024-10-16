rmt_dlv_checked_write(int fd, char id, char subid, void * buf, ssize_t size)
{
uschar pipe_header[PIPE_HEADER_SIZE+1];
size_t total_len = PIPE_HEADER_SIZE + size;

struct iovec iov[2] = {
  { pipe_header, PIPE_HEADER_SIZE },  /* indication about the data to expect */
  { buf, size }                       /* *the* data */
};

ssize_t ret;

/* we assume that size can't get larger then BIG_BUFFER_SIZE which currently is set to 16k */
/* complain to log if someone tries with buffer sizes we can't handle*/

if (size > BIG_BUFFER_SIZE-1)
  {
  log_write(0, LOG_MAIN|LOG_PANIC_DIE,
    "Failed writing transport result to pipe: can't handle buffers > %d bytes. truncating!\n",
      BIG_BUFFER_SIZE-1);
  size = BIG_BUFFER_SIZE;
  }

/* Should we check that we do not write more than PIPE_BUF? What would
that help? */

/* convert size to human readable string prepended by id and subid */
if (PIPE_HEADER_SIZE != snprintf(CS pipe_header, PIPE_HEADER_SIZE+1, "%c%c%05ld",
    id, subid, (long)size))
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "header snprintf failed\n");

DEBUG(D_deliver) debug_printf("header write id:%c,subid:%c,size:%ld,final:%s\n",
                                 id, subid, (long)size, pipe_header);

if ((ret = writev(fd, iov, 2)) != total_len)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE,
    "Failed writing transport result to pipe (%ld of %ld bytes): %s",
    (long)ret, (long)total_len, ret == -1 ? strerror(errno) : "short write");
}