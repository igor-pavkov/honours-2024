static void lo_getxattr(fuse_req_t req, fuse_ino_t ino, const char *in_name,
                        size_t size)
{
    struct lo_data *lo = lo_data(req);
    g_autofree char *value = NULL;
    char procname[64];
    const char *name;
    char *mapped_name;
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
            if (ret == -EPERM) {
                ret = -ENODATA;
            }
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

    fuse_log(FUSE_LOG_DEBUG, "lo_getxattr(ino=%" PRIu64 ", name=%s size=%zd)\n",
             ino, name, size);

    if (size) {
        value = g_try_malloc(size);
        if (!value) {
            goto out_err;
        }
    }

    sprintf(procname, "%i", inode->fd);
    /*
     * It is not safe to open() non-regular/non-dir files in file server
     * unless O_PATH is used, so use that method for regular files/dir
     * only (as it seems giving less performance overhead).
     * Otherwise, call fchdir() to avoid open().
     */
    if (S_ISREG(inode->filetype) || S_ISDIR(inode->filetype)) {
        fd = openat(lo->proc_self_fd, procname, O_RDONLY);
        if (fd < 0) {
            goto out_err;
        }
        ret = fgetxattr(fd, name, value, size);
        saverr = ret == -1 ? errno : 0;
    } else {
        /* fchdir should not fail here */
        FCHDIR_NOFAIL(lo->proc_self_fd);
        ret = getxattr(procname, name, value, size);
        saverr = ret == -1 ? errno : 0;
        FCHDIR_NOFAIL(lo->root.fd);
    }

    if (ret == -1) {
        goto out;
    }
    if (size) {
        saverr = 0;
        if (ret == 0) {
            goto out;
        }
        fuse_reply_buf(req, value, ret);
    } else {
        fuse_reply_xattr(req, ret);
    }
out_free:
    if (fd >= 0) {
        close(fd);
    }

    lo_inode_put(lo, &inode);
    return;

out_err:
    saverr = errno;
out:
    fuse_reply_err(req, saverr);
    g_free(mapped_name);
    goto out_free;
}