ssize_t pt_listxattr(FsContext *ctx, const char *path,
                     char *name, void *value, size_t size)
{
    int name_size = strlen(name) + 1;
    if (!value) {
        return name_size;
    }

    if (size < name_size) {
        errno = ERANGE;
        return -1;
    }

    /* no need for strncpy: name_size is strlen(name)+1 */
    memcpy(value, name, name_size);
    return name_size;
}