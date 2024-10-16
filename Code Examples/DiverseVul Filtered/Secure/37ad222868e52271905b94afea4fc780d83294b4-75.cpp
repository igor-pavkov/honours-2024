static void safe_fd_set(const int fd, fd_set * const fds)
{
    if (fd == -1) {
        return;
    }
    FD_SET(fd, fds);
}