void _pam_log(int err, const char *format, ...) {
    char msg[256];
    va_list args;

    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args);
    syslog(err, "PAM-tacplus: %s", msg);
    va_end(args);
}