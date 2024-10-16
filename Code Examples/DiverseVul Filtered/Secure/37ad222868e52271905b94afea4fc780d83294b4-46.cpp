void dotype(const char *arg)
{
    replycode = 200;            /* bloody awful hack */

    if (!arg || !*arg) {
        addreply(501, MSG_MISSING_ARG "\n" "A(scii) I(mage) L(ocal)");
    } else if (tolower((unsigned char) *arg) == 'a')
        type = 1;
    else if (tolower((unsigned char) *arg) == 'i')
        type = 2;
    else if (tolower((unsigned char) *arg) == 'l') {
        if (arg[1] == '8') {
            type = 2;
        } else if (isdigit((unsigned char) arg[1])) {
            addreply_noformat(504, MSG_TYPE_8BIT_FAILURE);
        } else {
            addreply_noformat(0, MSG_MISSING_ARG);
            type = 2;
        }
    } else {
        addreply(504, MSG_TYPE_UNKNOWN ": %s", arg);
    }

    addreply(0, MSG_TYPE_SUCCESS " %s", (type > 1) ? "8-bit binary" : "ASCII");
}