static void lo_link(fuse_req_t req, fuse_ino_t ino, fuse_ino_t parent,
                    const char *name)
{
    int res;
    struct lo_data *lo = lo_data(req);
    struct lo_inode *parent_inode;
    struct lo_inode *inode;
    struct fuse_entry_param e;
    char procname[64];
    int saverr;

    if (is_empty(name)) {
        fuse_reply_err(req, ENOENT);
        return;
    }

    if (!is_safe_path_component(name)) {
        fuse_reply_err(req, EINVAL);
        return;
    }

    parent_inode = lo_inode(req, parent);
    inode = lo_inode(req, ino);
    if (!parent_inode || !inode) {
        errno = EBADF;
        goto out_err;
    }

    memset(&e, 0, sizeof(struct fuse_entry_param));
    e.attr_timeout = lo->timeout;
    e.entry_timeout = lo->timeout;

    sprintf(procname, "%i", inode->fd);
    res = linkat(lo->proc_self_fd, procname, parent_inode->fd, name,
                 AT_SYMLINK_FOLLOW);
    if (res == -1) {
        goto out_err;
    }

    res = fstatat(inode->fd, "", &e.attr, AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW);
    if (res == -1) {
        goto out_err;
    }

    pthread_mutex_lock(&lo->mutex);
    inode->nlookup++;
    pthread_mutex_unlock(&lo->mutex);
    e.ino = inode->fuse_ino;

    fuse_log(FUSE_LOG_DEBUG, "  %lli/%s -> %lli\n", (unsigned long long)parent,
             name, (unsigned long long)e.ino);

    fuse_reply_entry(req, &e);
    lo_inode_put(lo, &parent_inode);
    lo_inode_put(lo, &inode);
    return;

out_err:
    saverr = errno;
    lo_inode_put(lo, &parent_inode);
    lo_inode_put(lo, &inode);
    fuse_reply_err(req, saverr);
}