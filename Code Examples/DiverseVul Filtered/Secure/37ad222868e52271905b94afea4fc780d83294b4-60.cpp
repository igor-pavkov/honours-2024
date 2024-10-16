int checkprintable(const char *s)
{
    int ret = 0;
    unsigned char c;

    while ((c = (unsigned char) *s) != 0U) {
        if (ISCTRLCODE(c)) {
            ret--;
            break;
        }
        s++;
    }

    return ret;
}