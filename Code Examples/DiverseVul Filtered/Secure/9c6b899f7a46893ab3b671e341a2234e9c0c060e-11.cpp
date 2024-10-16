static FILE *local_fopenat(int dirfd, const char *name, const char *mode)
{
    int fd, o_mode = 0;
    FILE *fp;
    int flags;
    /*
     * only supports two modes
     */
    if (mode[0] == 'r') {
        flags = O_RDONLY;
    } else if (mode[0] == 'w') {
        flags = O_WRONLY | O_TRUNC | O_CREAT;
        o_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    } else {
        return NULL;
    }
    fd = openat_file(dirfd, name, flags, o_mode);
    if (fd == -1) {
        return NULL;
    }
    fp = fdopen(fd, mode);
    if (!fp) {
        close(fd);
    }
    return fp;
}