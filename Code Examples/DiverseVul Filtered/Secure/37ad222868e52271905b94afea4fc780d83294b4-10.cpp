static void dns_sanitize(char *z)
{
    while (*z != 0) {
        if ((*z >= 'a' && *z <= 'z') ||
            (*z >= '0' && *z <= '9') ||
            *z == '.' || *z == '-' || *z == ':' ||
            (*z >= 'A' && *z <= 'Z')) {
            /* unless */
        } else {
            *z = '_';
        }
        z++;
    }
}