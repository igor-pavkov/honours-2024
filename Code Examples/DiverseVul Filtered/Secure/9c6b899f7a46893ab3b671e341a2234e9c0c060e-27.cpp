static ssize_t local_llistxattr(FsContext *ctx, V9fsPath *fs_path,
                                void *value, size_t size)
{
    char *path = fs_path->data;

    return v9fs_list_xattr(ctx, path, value, size);
}