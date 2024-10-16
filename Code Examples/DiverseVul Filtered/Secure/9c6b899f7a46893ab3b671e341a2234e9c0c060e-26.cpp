static void local_seekdir(FsContext *ctx, V9fsFidOpenState *fs, off_t off)
{
    seekdir(fs->dir.stream, off);
}