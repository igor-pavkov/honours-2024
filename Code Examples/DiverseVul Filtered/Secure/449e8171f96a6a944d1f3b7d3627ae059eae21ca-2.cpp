static void lo_rename(fuse_req_t req, fuse_ino_t parent, const char *name,
                      fuse_ino_t newparent, const char *newname,
                      unsigned int flags)
{
    int res;
    struct lo_inode *parent_inode;
    struct lo_inode *newparent_inode;
    struct lo_inode *oldinode = NULL;
    struct lo_inode *newinode = NULL;
    struct lo_data *lo = lo_data(req);

    if (is_empty(name) || is_empty(newname)) {
        fuse_reply_err(req, ENOENT);
        return;
    }

    if (!is_safe_path_component(name) || !is_safe_path_component(newname)) {
        fuse_reply_err(req, EINVAL);
        return;
    }

    parent_inode = lo_inode(req, parent);
    newparent_inode = lo_inode(req, newparent);
    if (!parent_inode || !newparent_inode) {
        fuse_reply_err(req, EBADF);
        goto out;
    }

    oldinode = lookup_name(req, parent, name);
    newinode = lookup_name(req, newparent, newname);

    if (!oldinode) {
        fuse_reply_err(req, EIO);
        goto out;
    }

    if (flags) {
#ifndef SYS_renameat2
        fuse_reply_err(req, EINVAL);
#else
        res = syscall(SYS_renameat2, parent_inode->fd, name,
                        newparent_inode->fd, newname, flags);
        if (res == -1 && errno == ENOSYS) {
            fuse_reply_err(req, EINVAL);
        } else {
            fuse_reply_err(req, res == -1 ? errno : 0);
        }
#endif
        goto out;
    }

    res = renameat(parent_inode->fd, name, newparent_inode->fd, newname);

    fuse_reply_err(req, res == -1 ? errno : 0);
out:
    unref_inode_lolocked(lo, oldinode, 1);
    unref_inode_lolocked(lo, newinode, 1);
    lo_inode_put(lo, &oldinode);
    lo_inode_put(lo, &newinode);
    lo_inode_put(lo, &parent_inode);
    lo_inode_put(lo, &newparent_inode);
}