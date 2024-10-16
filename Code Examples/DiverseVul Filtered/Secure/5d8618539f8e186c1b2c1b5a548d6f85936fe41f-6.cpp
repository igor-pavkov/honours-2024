static void x509_get_current_time( x509_time *now )
{
#if defined(_WIN32) && !defined(EFIX64) && !defined(EFI32)
    SYSTEMTIME st;

    GetSystemTime( &st );

    now->year = st.wYear;
    now->mon = st.wMonth;
    now->day = st.wDay;
    now->hour = st.wHour;
    now->min = st.wMinute;
    now->sec = st.wSecond;
#else
    struct tm lt;
    time_t tt;

    tt = time( NULL );
    gmtime_r( &tt, &lt );

    now->year = lt.tm_year + 1900;
    now->mon = lt.tm_mon + 1;
    now->day = lt.tm_mday;
    now->hour = lt.tm_hour;
    now->min = lt.tm_min;
    now->sec = lt.tm_sec;
#endif /* _WIN32 && !EFIX64 && !EFI32 */
}