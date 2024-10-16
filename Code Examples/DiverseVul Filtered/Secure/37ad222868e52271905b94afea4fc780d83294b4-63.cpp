void doutime(char *name, const char * const wanted_time)
{
    struct tm tm;
    time_t ts;
    struct utimbuf tb;

# ifndef ANON_CAN_CHANGE_UTIME
    if (guest != 0) {
        addreply_noformat(550, MSG_ANON_CANT_CHANGE_PERMS);
        return;
    }
# endif
    if (name == NULL || *name == 0) {
        addreply_noformat(501, MSG_NO_FILE_NAME);
        return;
    }
    if (checknamesanity(name, dot_write_ok) != 0) {
        addreply(553, MSG_SANITY_FILE_FAILURE, name);
        return;
    }
    memset(&tm, 0, sizeof tm);
    sscanf(wanted_time, "%4d%2d%2d%2d%2d%2d", &tm.tm_year, &tm.tm_mon,
           &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    tm.tm_mon--;
    tm.tm_year -= 1900;
# ifdef USE_LOCAL_TIME_FOR_SITE_UTIME
    ts = mktime(&tm);
# else
#  ifdef HAVE_TIMEGM
    ts = timegm(&tm);
#  elif defined(HAVE_PUTENV)
    {
        putenv("TZ=UTC+00:00");
#   ifdef HAVE_TZSET
        tzset();
#   endif
        ts = mktime(&tm);
        putenv(default_tz_for_putenv);
        tzset();
    }
#  else
    ts = mktime(&tm);
#  endif
# endif
    if (tm.tm_mon < 0 || tm.tm_year <= 0 || ts == (time_t) -1) {
        addreply_noformat(501, MSG_TIMESTAMP_FAILURE);
        return;
    }
    tb.actime = tb.modtime = ts;
    if (utime(name, &tb) < 0) {
        addreply(550, "utime(%s): %s", name, strerror(errno));
    } else {
        addreply_noformat(213, "UTIME OK");
    }
}