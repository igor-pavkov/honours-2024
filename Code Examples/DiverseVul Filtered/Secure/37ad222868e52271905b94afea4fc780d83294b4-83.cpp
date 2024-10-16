static void updatepidfile(void)
{
    int fd;
    char buf[42];
    size_t buf_len;

    if (SNCHECK(snprintf(buf, sizeof buf, "%lu\n",
                         (unsigned long) getpid()), sizeof buf)) {
        return;
    }
    if (unlink(pid_file) != 0 && errno != ENOENT) {
        return;
    }
    if ((fd = open(pid_file, O_CREAT | O_WRONLY | O_TRUNC |
                   O_NOFOLLOW, (mode_t) 0644)) == -1) {
        return;
    }
    buf_len = strlen(buf);
    if (safe_write(fd, buf, buf_len, -1) != (ssize_t) buf_len) {
        (void) ftruncate(fd, (off_t) 0);
    }
    (void) close(fd);
}