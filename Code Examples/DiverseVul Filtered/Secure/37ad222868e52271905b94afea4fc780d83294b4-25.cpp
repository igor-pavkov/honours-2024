static void displayopenfailure(const char * const name)
{
    char buffer[PATH_MAX + 42U];
    const int e = errno;

    if (SNCHECK(snprintf(buffer, sizeof buffer, MSG_OPEN_FAILURE, name),
                sizeof buffer)) {
        _EXIT(EXIT_FAILURE);
    }
    errno = e;
    error(550, buffer);
}