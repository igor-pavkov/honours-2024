static void lo_do_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
                          off_t offset, struct fuse_file_info *fi, int plus)
{
    struct lo_data *lo = lo_data(req);
    struct lo_dirp *d = NULL;
    struct lo_inode *dinode;
    g_autofree char *buf = NULL;
    char *p;
    size_t rem = size;
    int err = EBADF;

    dinode = lo_inode(req, ino);
    if (!dinode) {
        goto error;
    }

    d = lo_dirp(req, fi);
    if (!d) {
        goto error;
    }

    err = ENOMEM;
    buf = g_try_malloc0(size);
    if (!buf) {
        goto error;
    }
    p = buf;

    if (offset != d->offset) {
        seekdir(d->dp, offset);
        d->entry = NULL;
        d->offset = offset;
    }
    while (1) {
        size_t entsize;
        off_t nextoff;
        const char *name;

        if (!d->entry) {
            errno = 0;
            d->entry = readdir(d->dp);
            if (!d->entry) {
                if (errno) { /* Error */
                    err = errno;
                    goto error;
                } else { /* End of stream */
                    break;
                }
            }
        }
        nextoff = d->entry->d_off;
        name = d->entry->d_name;

        fuse_ino_t entry_ino = 0;
        struct fuse_entry_param e = (struct fuse_entry_param){
            .attr.st_ino = d->entry->d_ino,
            .attr.st_mode = d->entry->d_type << 12,
        };

        /* Hide root's parent directory */
        if (dinode == &lo->root && strcmp(name, "..") == 0) {
            e.attr.st_ino = lo->root.key.ino;
            e.attr.st_mode = DT_DIR << 12;
        }

        if (plus) {
            if (!is_dot_or_dotdot(name)) {
                err = lo_do_lookup(req, ino, name, &e, NULL);
                if (err) {
                    goto error;
                }
                entry_ino = e.ino;
            }

            entsize = fuse_add_direntry_plus(req, p, rem, name, &e, nextoff);
        } else {
            entsize = fuse_add_direntry(req, p, rem, name, &e.attr, nextoff);
        }
        if (entsize > rem) {
            if (entry_ino != 0) {
                lo_forget_one(req, entry_ino, 1);
            }
            break;
        }

        p += entsize;
        rem -= entsize;

        d->entry = NULL;
        d->offset = nextoff;
    }

    err = 0;
error:
    lo_dirp_put(&d);
    lo_inode_put(lo, &dinode);

    /*
     * If there's an error, we can only signal it if we haven't stored
     * any entries yet - otherwise we'd end up with wrong lookup
     * counts for the entries that are already in the buffer. So we
     * return what we've collected until that point.
     */
    if (err && rem == size) {
        fuse_reply_err(req, err);
    } else {
        fuse_reply_buf(req, buf, size - rem);
    }
}