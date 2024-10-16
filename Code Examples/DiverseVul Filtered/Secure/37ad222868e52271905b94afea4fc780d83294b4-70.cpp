void error(int n, const char *msg)
{
    const char *e = strerror(errno);

    logfile(LOG_ERR, "%s: %s", msg, e);
    addreply(n, "%s: %s", msg, e);
}