char *skip_telnet_controls(const char *str)
{
    if (str == NULL) {
        return NULL;
    }
    while (*str != 0 && (unsigned char) *str >= 240U) {
        str++;
    }
    return (char *) str;
}