static void lo_mknod_symlink(fuse_req_t req, fuse_ino_t parent,
                             const char *name, mode_t mode, dev_t rdev,
                             const char *link)
{
    int res;
    int saverr;
    struct lo_data *lo = lo_data(req);
    struct lo_inode *dir;
    struct fuse_entry_param e;
    struct lo_cred old = {};

    if (is_empty(name)) {
        fuse_reply_err(req, ENOENT);
        return;
    }

    if (!is_safe_path_component(name)) {
        fuse_reply_err(req, EINVAL);
        return;
    }

    dir = lo_inode(req, parent);
    if (!dir) {
        fuse_reply_err(req, EBADF);
        return;
    }

    saverr = lo_change_cred(req, &old, lo->change_umask && !S_ISLNK(mode));
    if (saverr) {
        goto out;
    }

    res = mknod_wrapper(dir->fd, name, link, mode, rdev);

    saverr = errno;

    lo_restore_cred(&old, lo->change_umask && !S_ISLNK(mode));

    if (res == -1) {
        goto out;
    }

    saverr = lo_do_lookup(req, parent, name, &e, NULL);
    if (saverr) {
        goto out;
    }

    fuse_log(FUSE_LOG_DEBUG, "  %lli/%s -> %lli\n", (unsigned long long)parent,
             name, (unsigned long long)e.ino);

    fuse_reply_entry(req, &e);
    lo_inode_put(lo, &dir);
    return;

out:
    lo_inode_put(lo, &dir);
    fuse_reply_err(req, saverr);
}