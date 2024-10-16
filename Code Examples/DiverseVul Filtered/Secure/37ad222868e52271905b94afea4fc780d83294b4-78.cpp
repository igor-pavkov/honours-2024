void dobanner(const int type)
{
    char buffer[512];
    FILE *msg;
    size_t buflen;
    unsigned int nblines = BANNER_MAXLINES;

    switch (type) {
    case 0:
        if ((msg = fopen(".banner", "r")) == NULL
# ifdef WITH_WELCOME_MSG
            && (msg = fopen("welcome.msg", "r")) == NULL
# endif
            ) {
            return;
        }
        break;
    case 1:
        if ((msg = fopen(".message", "r")) == NULL) {
            return;
        }
        break;
    default:
        return;
    }

    while (fgets(buffer, sizeof buffer, msg) != NULL && nblines > 0U) {
        nblines--;
        if ((buflen = strlen(buffer)) > (size_t) 0U) {
            buflen--;
            while (buffer[buflen] == '\n' || buffer[buflen] == '\r') {
                buffer[buflen] = 0;
                if (buflen == (size_t) 0U) {
                    break;
                }
                buflen--;
            }
            stripctrl(buffer, buflen);
        }
        addreply_noformat(0, buffer);
    }
    (void) fclose(msg);
}