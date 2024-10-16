static void unlinkat_preserve_errno(int dirfd, const char *path, int flags)
{
    int serrno = errno;
    unlinkat(dirfd, path, flags);
    errno = serrno;
}