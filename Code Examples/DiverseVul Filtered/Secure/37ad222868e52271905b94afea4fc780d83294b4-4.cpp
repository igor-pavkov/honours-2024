void dositetime(void)
{
    char tmp[64];
    const struct tm *tm;
    time_t now;

    if ((now = time(NULL)) == (time_t) -1 || (tm = localtime(&now)) == NULL) {
        addreply_noformat(451, "time()");
        return;
    }
    strftime(tmp, sizeof tmp, "%Y-%m-%d %H:%M:%S", tm);
    addreply_noformat(211, tmp);
}