static int init_tz(void)
{
    char stbuf[10];
    struct tm *tm;
    time_t now = time(NULL);

#ifdef HAVE_TZSET
    tzset();
#endif
#ifdef HAVE_PUTENV
    if ((tm = localtime(&now)) != NULL &&
        strftime(stbuf, sizeof stbuf, "%z", tm) == (size_t) 5U) {
        snprintf(default_tz_for_putenv, sizeof default_tz_for_putenv,
                 "TZ=UTC%c%c%c:%c%c", (*stbuf == '-' ? '+' : '-'),
                 stbuf[1], stbuf[2], stbuf[3], stbuf[4]);
    }
    putenv(default_tz_for_putenv);
#endif
    (void) localtime(&now);
    (void) gmtime(&now);

    return 0;
}