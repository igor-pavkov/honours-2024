void domode(const char *arg)
{
    if (arg == NULL || !*arg) {
        addreply_noformat(501, MSG_MISSING_ARG);
    } else if (strcasecmp(arg, "S")) {
        addreply_noformat(504, MSG_MODE_FAILURE);
    } else {
        addreply_noformat(200, "S OK");
    }
}