static void local_rewinddir(FsContext *ctx, V9fsFidOpenState *fs)
{
    rewinddir(fs->dir.stream);
}