static void lo_rmdir(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    int res;
    struct lo_inode *inode;
    struct lo_data *lo = lo_data(req);

    if (is_empty(name)) {
        fuse_reply_err(req, ENOENT);
        return;
    }

    if (!is_safe_path_component(name)) {
        fuse_reply_err(req, EINVAL);
        return;
    }

    inode = lookup_name(req, parent, name);
    if (!inode) {
        fuse_reply_err(req, EIO);
        return;
    }

    res = unlinkat(lo_fd(req, parent), name, AT_REMOVEDIR);

    fuse_reply_err(req, res == -1 ? errno : 0);
    unref_inode_lolocked(lo, inode, 1);
    lo_inode_put(lo, &inode);
}