static double get_usec_time(void)
{
    struct timeval tv;
    struct timezone tz;

    if (gettimeofday(&tv, &tz) < 0) {
        return 0.0;
    }
    return (double) tv.tv_sec + ((double) tv.tv_usec) / 1000000.0;
}