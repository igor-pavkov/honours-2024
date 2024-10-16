static int local_init(FsContext *ctx)
{
    struct statfs stbuf;
    LocalData *data = g_malloc(sizeof(*data));

    data->mountfd = open(ctx->fs_root, O_DIRECTORY | O_RDONLY);
    if (data->mountfd == -1) {
        goto err;
    }

#ifdef FS_IOC_GETVERSION
    /*
     * use ioc_getversion only if the ioctl is definied
     */
    if (fstatfs(data->mountfd, &stbuf) < 0) {
        close_preserve_errno(data->mountfd);
        goto err;
    }
    switch (stbuf.f_type) {
    case EXT2_SUPER_MAGIC:
    case BTRFS_SUPER_MAGIC:
    case REISERFS_SUPER_MAGIC:
    case XFS_SUPER_MAGIC:
        ctx->exops.get_st_gen = local_ioc_getversion;
        break;
    }
#endif

    if (ctx->export_flags & V9FS_SM_PASSTHROUGH) {
        ctx->xops = passthrough_xattr_ops;
    } else if (ctx->export_flags & V9FS_SM_MAPPED) {
        ctx->xops = mapped_xattr_ops;
    } else if (ctx->export_flags & V9FS_SM_NONE) {
        ctx->xops = none_xattr_ops;
    } else if (ctx->export_flags & V9FS_SM_MAPPED_FILE) {
        /*
         * xattr operation for mapped-file and passthrough
         * remain same.
         */
        ctx->xops = passthrough_xattr_ops;
    }
    ctx->export_flags |= V9FS_PATHNAME_FSCONTEXT;

    ctx->private = data;
    return 0;

err:
    g_free(data);
    return -1;
}