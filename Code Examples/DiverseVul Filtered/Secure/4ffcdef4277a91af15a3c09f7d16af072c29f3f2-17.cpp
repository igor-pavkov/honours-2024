ssize_t notsup_getxattr(FsContext *ctx, const char *path, const char *name,
                        void *value, size_t size)
{
    errno = ENOTSUP;
    return -1;
}