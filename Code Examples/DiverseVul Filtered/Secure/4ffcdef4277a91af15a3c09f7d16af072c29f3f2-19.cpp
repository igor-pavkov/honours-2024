static ssize_t flistxattrat_nofollow(int dirfd, const char *filename,
                                     char *list, size_t size)
{
    char *proc_path = g_strdup_printf("/proc/self/fd/%d/%s", dirfd, filename);
    int ret;

    ret = llistxattr(proc_path, list, size);
    g_free(proc_path);
    return ret;
}