int fsetxattrat_nofollow(int dirfd, const char *filename, const char *name,
                         void *value, size_t size, int flags)
{
    char *proc_path = g_strdup_printf("/proc/self/fd/%d/%s", dirfd, filename);
    int ret;

    ret = lsetxattr(proc_path, name, value, size, flags);
    g_free(proc_path);
    return ret;
}