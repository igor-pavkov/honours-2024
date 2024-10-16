int notsup_removexattr(FsContext *ctx, const char *path, const char *name)
{
    errno = ENOTSUP;
    return -1;
}