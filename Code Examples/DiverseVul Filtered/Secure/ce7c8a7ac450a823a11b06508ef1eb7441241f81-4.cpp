int _q_countread(const char *filepath)
{
    int fd = open(filepath, O_RDONLY, 0);
    if (fd < 0) return 0;

    char buf[10+1];
    ssize_t readed = read(fd, buf, (sizeof(buf) - 1));
    close(fd);

    if (readed > 0) {
        buf[readed] = '\0';
        return atoi(buf);
    }
    return 0;
}