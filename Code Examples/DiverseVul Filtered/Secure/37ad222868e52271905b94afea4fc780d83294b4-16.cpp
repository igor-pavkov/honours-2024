void dostru(const char *arg)
{
    if (arg == NULL || !*arg) {
        addreply_noformat(501, MSG_MISSING_ARG);
    } else if (strcasecmp(arg, "F")) {
        addreply_noformat(504, MSG_STRU_FAILURE);
    } else {
        addreply_noformat(200, "F OK");
    }
}