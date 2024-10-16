ssize_t load_image_size(const char *filename, void *addr, size_t size)
{
    int fd;
    ssize_t actsize, l = 0;

    fd = open(filename, O_RDONLY | O_BINARY);
    if (fd < 0) {
        return -1;
    }

    while ((actsize = read(fd, addr + l, size - l)) > 0) {
        l += actsize;
    }

    close(fd);

    return actsize < 0 ? -1 : l;
}