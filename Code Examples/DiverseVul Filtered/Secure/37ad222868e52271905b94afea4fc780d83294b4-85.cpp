void stripctrl(char * const buf, size_t len)
{
    if (len <= (size_t) 0U) {
        return;
    }
    do {
        len--;
        if (ISCTRLCODE(buf[len]) &&
            buf[len] != 0 && buf[len] != '\n') {
            buf[len] = '_';
        }
    } while (len != (size_t) 0U);
}