static void sigterm(int sig)
{
    const int olderrno = errno;
    (void) sig;

    stop_server = 1;
    if (listenfd != -1) {
        shutdown(listenfd, 2);
        (void) close(listenfd);
    }
    if (listenfd6 != -1) {
        shutdown(listenfd6, 2);
        (void) close(listenfd6);
    }
    errno = olderrno;
}