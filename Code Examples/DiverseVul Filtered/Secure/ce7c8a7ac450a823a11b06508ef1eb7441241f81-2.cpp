long _qunit_current_milli(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long time = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    return time;
}