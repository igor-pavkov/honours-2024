static off_t local_telldir(FsContext *ctx, V9fsFidOpenState *fs)
{
    return telldir(fs->dir.stream);
}