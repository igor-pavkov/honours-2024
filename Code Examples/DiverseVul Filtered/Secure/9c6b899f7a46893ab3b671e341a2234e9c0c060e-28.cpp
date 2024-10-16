static int local_lremovexattr(FsContext *ctx, V9fsPath *fs_path,
                              const char *name)
{
    char *path = fs_path->data;

    return v9fs_remove_xattr(ctx, path, name);
}