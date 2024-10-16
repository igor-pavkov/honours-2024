static ssize_t fremovexattrat_nofollow(int dirfd, const char *filename,
                                       const char *name)
{
    char *proc_path = g_strdup_printf("/proc/self/fd/%d/%s", dirfd, filename);
    int ret;

    ret = lremovexattr(proc_path, name);
    g_free(proc_path);
    return ret;
}