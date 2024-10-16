static void setup_sandbox(struct lo_data *lo, struct fuse_session *se,
                          bool enable_syslog)
{
    setup_namespaces(lo, se);
    setup_mounts(lo->source);
    setup_seccomp(enable_syslog);
    setup_capabilities(g_strdup(lo->modcaps));
}