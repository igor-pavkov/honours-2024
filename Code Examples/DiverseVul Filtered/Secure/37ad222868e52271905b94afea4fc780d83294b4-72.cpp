void addreply(const int code, const char * const line, ...)
{
    char *a;
    char *b;
    va_list ap;
    int last;
    char buf[MAX_SERVER_REPLY_LEN];

    if (code != 0) {
        replycode = code;
    }
    va_start(ap, line);
    vsnprintf(buf, sizeof buf, line, ap);
    va_end(ap);
    last = 0;
    a = buf;
    for (;;) {
        b = strchr(a, '\n');
        if (b != NULL) {
            *b = 0;
        } else {
            b = a;
            while (*b != 0) {
                b++;
            }
            last++;
        }
        addreply_newline(a, (size_t) (b - a) + (size_t) 1U);
        if (last != 0) {
            break;
        }
        a = b + 1;
    }
}