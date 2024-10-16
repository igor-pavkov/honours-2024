int local_opendir_nofollow(FsContext *fs_ctx, const char *path)
{
    return local_open_nofollow(fs_ctx, path, O_DIRECTORY | O_RDONLY, 0);
}