static unsigned int open_max(void)
{
    long z;

    if ((z = (long) sysconf(_SC_OPEN_MAX)) < 0L) {
        perror("_SC_OPEN_MAX");
        _EXIT(EXIT_FAILURE);
    }
    return (unsigned int) z;
}