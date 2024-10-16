static void lo_removexattr(fuse_req_t req, fuse_ino_t ino, const char *in_name)
{
    char procname[64];
    const char *name;
    char *mapped_name;
    struct lo_data *lo = lo_data(req);
    struct lo_inode *inode;
    ssize_t ret;
    int saverr;
    int fd = -1;

    if (block_xattr(lo, in_name)) {
        fuse_reply_err(req, EOPNOTSUPP);
        return;
    }

    mapped_name = NULL;
    name = in_name;
    if (lo->xattrmap) {
        ret = xattr_map_client(lo, in_name, &mapped_name);
        if (ret < 0) {
            fuse_reply_err(req, -ret);
            return;
        }
        if (mapped_name) {
            name = mapped_name;
        }
    }

    inode = lo_inode(req, ino);
    if (!inode) {
        fuse_reply_err(req, EBADF);
        g_free(mapped_name);
        return;
    }

    saverr = ENOSYS;
    if (!lo_data(req)->xattr) {
        goto out;
    }

    fuse_log(FUSE_LOG_DEBUG, "lo_removexattr(ino=%" PRIu64 ", name=%s)\n", ino,
             name);

    sprintf(procname, "%i", inode->fd);
    if (S_ISREG(inode->filetype) || S_ISDIR(inode->filetype)) {
        fd = openat(lo->proc_self_fd, procname, O_RDONLY);
        if (fd < 0) {
            saverr = errno;
            goto out;
        }
        ret = fremovexattr(fd, name);
        saverr = ret == -1 ? errno : 0;
    } else {
        /* fchdir should not fail here */
        FCHDIR_NOFAIL(lo->proc_self_fd);
        ret = removexattr(procname, name);
        saverr = ret == -1 ? errno : 0;
        FCHDIR_NOFAIL(lo->root.fd);
    }

out:
    if (fd >= 0) {
        close(fd);
    }

    lo_inode_put(lo, &inode);
    g_free(mapped_name);
    fuse_reply_err(req, saverr);
}