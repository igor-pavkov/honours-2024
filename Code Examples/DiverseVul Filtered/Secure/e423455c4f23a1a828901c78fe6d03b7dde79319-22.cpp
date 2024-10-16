static void *load_at(int fd, off_t offset, size_t size)
{
    void *ptr;
    if (lseek(fd, offset, SEEK_SET) < 0)
        return NULL;
    ptr = g_malloc(size);
    if (read(fd, ptr, size) != size) {
        g_free(ptr);
        return NULL;
    }
    return ptr;
}