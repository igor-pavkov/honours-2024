void die(const int err, const int priority, const char * const format, ...)
{
    va_list va;
    char line[MAX_SYSLOG_LINE];

    disablesignals();
    logging = 0;
    va_start(va, format);
    vsnprintf(line, sizeof line, format, va);
    addreply(err, "%s", line);
    va_end(va);
    doreply();
    logfile(priority, "%s", line);
    _EXIT(-priority - 1);
}