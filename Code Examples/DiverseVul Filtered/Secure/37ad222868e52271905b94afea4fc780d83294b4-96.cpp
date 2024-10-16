static int safe_fd_isset(const int fd, const fd_set * const fds)
{
    if (fd == -1) {
        return 0;
    }
    return FD_ISSET(fd, fds);
}