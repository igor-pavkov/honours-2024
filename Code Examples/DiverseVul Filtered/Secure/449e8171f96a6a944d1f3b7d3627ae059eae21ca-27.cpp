static void lo_lookup(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    struct fuse_entry_param e;
    int err;

    fuse_log(FUSE_LOG_DEBUG, "lo_lookup(parent=%" PRIu64 ", name=%s)\n", parent,
             name);

    if (is_empty(name)) {
        fuse_reply_err(req, ENOENT);
        return;
    }

    /*
     * Don't use is_safe_path_component(), allow "." and ".." for NFS export
     * support.
     */
    if (strchr(name, '/')) {
        fuse_reply_err(req, EINVAL);
        return;
    }

    err = lo_do_lookup(req, parent, name, &e, NULL);
    if (err) {
        fuse_reply_err(req, err);
    } else {
        fuse_reply_entry(req, &e);
    }
}