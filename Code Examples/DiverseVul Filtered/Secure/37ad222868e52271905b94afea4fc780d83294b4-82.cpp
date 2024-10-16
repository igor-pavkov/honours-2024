void dostou(void)
{
    char file[64];
    static unsigned int seq = 0U;
    struct timeval tv;
    struct timezone tz;

    if (gettimeofday(&tv, &tz) != 0) {
        error(553, MSG_TIMESTAMP_FAILURE);
        return;
    }
    if (SNCHECK(snprintf(file, sizeof file, "pureftpd.%08lx.%02lx.%04x",
                         (unsigned long) tv.tv_sec,
                         (unsigned long) tv.tv_usec & 0xff,
                         seq), sizeof file)) {
        _EXIT(EXIT_FAILURE);
    }
    seq++;
    seq &= 0xffff;
    addreply(150, "FILE: %s", file);
    dostor(file, 0, 1);
}