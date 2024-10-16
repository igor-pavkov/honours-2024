static void renameat_preserve_errno(int odirfd, const char *opath, int ndirfd,
                                    const char *npath)
{
    int serrno = errno;
    renameat(odirfd, opath, ndirfd, npath);
    errno = serrno;
}