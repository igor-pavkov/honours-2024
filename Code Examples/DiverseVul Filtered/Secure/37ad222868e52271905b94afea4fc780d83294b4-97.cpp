void domdtm(const char *name)
{
    struct stat st;
    struct tm *t;

    if (!name || !*name) {
        addreply_noformat(501, MSG_MISSING_ARG);
    } else if (stat(name, &st)) {
#ifdef DEBUG
        if (debug != 0) {
            addreply(0, "arg: %s, wd: %s", name, wd);
        }
#endif
        addreply_noformat(550, MSG_STAT_FAILURE2);
    } else if (!S_ISREG(st.st_mode)) {
        addreply_noformat(550, MSG_NOT_REGULAR_FILE);
    } else {
        t = gmtime((time_t *) &(st.st_mtime));
        if (!t) {
            addreply_noformat(451, MSG_GMTIME_FAILURE);
        } else {
            addreply(213, "%04d%02d%02d%02d%02d%02d",
                     t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                     t->tm_hour, t->tm_min, t->tm_sec);
        }
    }
}