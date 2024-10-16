static double get_time (void) {
#ifdef DO_TIME_DAY
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return tv.tv_sec + 1e-6 * tv.tv_usec;
#else
    struct timeb tb;

    ftime(&tb);

    return tb.time + 1e-3 * tb.millitm;
#endif
}