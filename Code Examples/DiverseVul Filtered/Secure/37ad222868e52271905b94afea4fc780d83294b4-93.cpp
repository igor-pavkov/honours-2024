void dorest(const char *name)
{
    char *endptr;

    restartat = (off_t) strtoull(name, &endptr, 10);
    if (*endptr != 0 || restartat < (off_t) 0) {
        restartat = 0;
        addreply(554, MSG_REST_NOT_NUMERIC "\n" MSG_REST_RESET);
    } else {
        if (type == 1 && restartat != 0) {
#ifdef STRICT_REST
            addreply_noformat(504, MSG_REST_ASCII_STRICT);
#else
            addreply(350, MSG_REST_ASCII_WORKAROUND,
                     (long long) restartat);
#endif
        } else {
            if (restartat != 0) {
                logfile(LOG_NOTICE, MSG_REST_SUCCESS, (long long) restartat);
            }
            addreply(350, MSG_REST_SUCCESS, (long long) restartat);
        }
    }
}