int notsup_setxattr(FsContext *ctx, const char *path, const char *name,
                    void *value, size_t size, int flags)
{
    errno = ENOTSUP;
    return -1;
}