static int lo_drop_cap_change_cred(fuse_req_t req, struct lo_cred *old,
                                   bool change_umask, const char *cap_name,
                                   bool *cap_dropped)
{
    int ret;
    bool __cap_dropped;

    assert(cap_name);

    ret = drop_effective_cap(cap_name, &__cap_dropped);
    if (ret) {
        return ret;
    }

    ret = lo_change_cred(req, old, change_umask);
    if (ret) {
        if (__cap_dropped) {
            if (gain_effective_cap(cap_name)) {
                fuse_log(FUSE_LOG_ERR, "Failed to gain CAP_%s\n", cap_name);
            }
        }
    }

    if (cap_dropped) {
        *cap_dropped = __cap_dropped;
    }
    return ret;
}