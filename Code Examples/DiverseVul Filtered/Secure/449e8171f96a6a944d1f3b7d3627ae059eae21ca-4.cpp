static void lo_setxattr(fuse_req_t req, fuse_ino_t ino, const char *in_name,
                        const char *value, size_t size, int flags,
                        uint32_t extra_flags)
{
    char procname[64];
    const char *name;
    char *mapped_name;
    struct lo_data *lo = lo_data(req);
    struct lo_inode *inode;
    ssize_t ret;
    int saverr;
    int fd = -1;
    bool switched_creds = false;
    bool cap_fsetid_dropped = false;
    struct lo_cred old = {};

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

    fuse_log(FUSE_LOG_DEBUG, "lo_setxattr(ino=%" PRIu64
             ", name=%s value=%s size=%zd)\n", ino, name, value, size);

    sprintf(procname, "%i", inode->fd);
    /*
     * If we are setting posix access acl and if SGID needs to be
     * cleared, then switch to caller's gid and drop CAP_FSETID
     * and that should make sure host kernel clears SGID.
     *
     * This probably will not work when we support idmapped mounts.
     * In that case we will need to find a non-root gid and switch
     * to it. (Instead of gid in request). Fix it when we support
     * idmapped mounts.
     */
    if (lo->posix_acl && !strcmp(name, "system.posix_acl_access")
        && (extra_flags & FUSE_SETXATTR_ACL_KILL_SGID)) {
        ret = lo_drop_cap_change_cred(req, &old, false, "FSETID",
                                      &cap_fsetid_dropped);
        if (ret) {
            saverr = ret;
            goto out;
        }
        switched_creds = true;
    }
    if (S_ISREG(inode->filetype) || S_ISDIR(inode->filetype)) {
        fd = openat(lo->proc_self_fd, procname, O_RDONLY);
        if (fd < 0) {
            saverr = errno;
            goto out;
        }
        ret = fsetxattr(fd, name, value, size, flags);
        saverr = ret == -1 ? errno : 0;
    } else {
        /* fchdir should not fail here */
        FCHDIR_NOFAIL(lo->proc_self_fd);
        ret = setxattr(procname, name, value, size, flags);
        saverr = ret == -1 ? errno : 0;
        FCHDIR_NOFAIL(lo->root.fd);
    }
    if (switched_creds) {
        if (cap_fsetid_dropped)
            lo_restore_cred_gain_cap(&old, false, "FSETID");
        else
            lo_restore_cred(&old, false);
    }

out:
    if (fd >= 0) {
        close(fd);
    }

    lo_inode_put(lo, &inode);
    g_free(mapped_name);
    fuse_reply_err(req, saverr);
}