ssize_t notsup_listxattr(FsContext *ctx, const char *path, char *name,
                         void *value, size_t size)
{
    return 0;
}