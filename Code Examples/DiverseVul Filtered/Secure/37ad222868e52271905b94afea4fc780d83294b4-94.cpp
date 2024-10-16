void dosize(const char *name)
{
    struct stat st;

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
        addreply(213, "%llu", (unsigned long long) st.st_size);
    }
}