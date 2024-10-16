static struct lo_inode *lookup_name(fuse_req_t req, fuse_ino_t parent,
                                    const char *name)
{
    int res;
    uint64_t mnt_id;
    struct stat attr;
    struct lo_data *lo = lo_data(req);
    struct lo_inode *dir = lo_inode(req, parent);

    if (!dir) {
        return NULL;
    }

    res = do_statx(lo, dir->fd, name, &attr, AT_SYMLINK_NOFOLLOW, &mnt_id);
    lo_inode_put(lo, &dir);
    if (res == -1) {
        return NULL;
    }

    return lo_find(lo, &attr, mnt_id);
}