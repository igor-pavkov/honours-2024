bool _q_countsave(const char *filepath, int number)
{
    int fd = open(filepath, O_CREAT|O_WRONLY|O_TRUNC, DEF_FILE_MODE);
    if (fd < 0) return false;

    char buf[10+1];
    snprintf(buf, sizeof(buf), "%d", number);
    ssize_t updated = write(fd, buf, strlen(buf));
    close(fd);

    if (updated > 0) return true;
    return false;
}