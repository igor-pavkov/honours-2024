static void set_cloexec_flag(const int fd)
{
    fcntl(fd, F_SETFD, FD_CLOEXEC);
}