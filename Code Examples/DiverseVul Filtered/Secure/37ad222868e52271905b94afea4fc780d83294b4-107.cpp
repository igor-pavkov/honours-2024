static void overlapcpy(char *d, const char *s)
{
    while (*s != 0) {
        *d++ = *s++;
    }
    *d = 0;
}