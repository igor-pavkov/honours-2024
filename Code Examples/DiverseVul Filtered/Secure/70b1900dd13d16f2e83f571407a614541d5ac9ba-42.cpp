static void checkfds(void)
{
#ifdef HAVE_PIPE
  int fd[2] = { STDIN_FILENO, STDIN_FILENO };
  while( fd[0] == STDIN_FILENO ||
         fd[0] == STDOUT_FILENO ||
         fd[0] == STDERR_FILENO ||
         fd[1] == STDIN_FILENO ||
         fd[1] == STDOUT_FILENO ||
         fd[1] == STDERR_FILENO )
    if (pipe(fd) < 0)
      return;   /* Out of handles. This isn't really a big problem now, but
                   will be when we try to create a socket later. */
  close(fd[0]);
  close(fd[1]);
#endif
}