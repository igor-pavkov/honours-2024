void logfile(const int crit, const char *format, ...)
{
#if defined(NON_ROOT_FTP)
    (void) crit;
    (void) format;
#else
    const char *urgency;
    va_list va;
    char line[MAX_SYSLOG_LINE];

    if (no_syslog != 0) {
        va_end(va);
        return;
    }
    va_start(va, format);
    vsnprintf(line, sizeof line, format, va);
    va_end(va);
    switch (crit) {
    case LOG_INFO:
        urgency = "[INFO] ";
        break;
    case LOG_WARNING:
        urgency = "[WARNING] ";
        break;
    case LOG_ERR:
        urgency = "[ERROR] ";
        break;
    case LOG_NOTICE:
        urgency = "[NOTICE] ";
        break;
    case LOG_DEBUG:
        urgency = "[DEBUG] ";
        break;
    default:
        urgency = "";
    }
# ifdef SAVE_DESCRIPTORS
    openlog("pure-ftpd", log_pid, syslog_facility);
# endif
    syslog(crit, "(%s@%s) %s%s",
           ((loggedin != 0 && *account != 0) ? account : "?"),
           (*host != 0 ? host : "?"),
           urgency, line);
# ifdef SAVE_DESCRIPTORS
    closelog();
# endif
#endif
}