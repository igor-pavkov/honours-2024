static int local_close(FsContext *ctx, V9fsFidOpenState *fs)
{
    return close(fs->fd);
}