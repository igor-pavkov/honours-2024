static long sysconf_exact(int name, const char *name_str, Error **err)
{
    long ret;

    errno = 0;
    ret = sysconf(name);
    if (ret == -1) {
        if (errno == 0) {
            error_setg(err, "sysconf(%s): value indefinite", name_str);
        } else {
            error_setg_errno(err, errno, "sysconf(%s)", name_str);
        }
    }
    return ret;
}