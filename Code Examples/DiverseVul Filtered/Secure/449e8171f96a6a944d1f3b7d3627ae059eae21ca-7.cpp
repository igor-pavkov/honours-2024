static void lo_listxattr(fuse_req_t req, fuse_ino_t ino, size_t size)
{
    struct lo_data *lo = lo_data(req);
    g_autofree char *value = NULL;
    char procname[64];
    struct lo_inode *inode;
    ssize_t ret;
    int saverr;
    int fd = -1;

    inode = lo_inode(req, ino);
    if (!inode) {
        fuse_reply_err(req, EBADF);
        return;
    }

    saverr = ENOSYS;
    if (!lo_data(req)->xattr) {
        goto out;
    }

    fuse_log(FUSE_LOG_DEBUG, "lo_listxattr(ino=%" PRIu64 ", size=%zd)\n", ino,
             size);

    if (size) {
        value = g_try_malloc(size);
        if (!value) {
            goto out_err;
        }
    }

    sprintf(procname, "%i", inode->fd);
    if (S_ISREG(inode->filetype) || S_ISDIR(inode->filetype)) {
        fd = openat(lo->proc_self_fd, procname, O_RDONLY);
        if (fd < 0) {
            goto out_err;
        }
        ret = flistxattr(fd, value, size);
        saverr = ret == -1 ? errno : 0;
    } else {
        /* fchdir should not fail here */
        FCHDIR_NOFAIL(lo->proc_self_fd);
        ret = listxattr(procname, value, size);
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

        if (lo->xattr_map_list) {
            /*
             * Map the names back, some attributes might be dropped,
             * some shortened, but not increased, so we shouldn't
             * run out of room.
             */
            size_t out_index, in_index;
            out_index = 0;
            in_index = 0;
            while (in_index < ret) {
                const char *map_out;
                char *in_ptr = value + in_index;
                /* Length of current attribute name */
                size_t in_len = strlen(value + in_index) + 1;

                int mapret = xattr_map_server(lo, in_ptr, &map_out);
                if (mapret != -ENODATA && mapret != 0) {
                    /* Shouldn't happen */
                    saverr = -mapret;
                    goto out;
                }
                if (mapret == 0) {
                    /* Either unchanged, or truncated */
                    size_t out_len;
                    if (map_out != in_ptr) {
                        /* +1 copies the NIL */
                        out_len = strlen(map_out) + 1;
                    } else {
                        /* No change */
                        out_len = in_len;
                    }
                    /*
                     * Move result along, may still be needed for an unchanged
                     * entry if a previous entry was changed.
                     */
                    memmove(value + out_index, map_out, out_len);

                    out_index += out_len;
                }
                in_index += in_len;
            }
            ret = out_index;
            if (ret == 0) {
                goto out;
            }
        }

        ret = remove_blocked_xattrs(lo, value, ret);
        if (ret <= 0) {
            saverr = -ret;
            goto out;
        }
        fuse_reply_buf(req, value, ret);
    } else {
        /*
         * xattrmap only ever shortens the result,
         * so we don't need to do anything clever with the
         * allocation length here.
         */
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
    goto out_free;
}