int bdrv_parse_discard_flags(const char *mode, int *flags)
{
    *flags &= ~BDRV_O_UNMAP;

    if (!strcmp(mode, "off") || !strcmp(mode, "ignore")) {
        /* do nothing */
    } else if (!strcmp(mode, "on") || !strcmp(mode, "unmap")) {
        *flags |= BDRV_O_UNMAP;
    } else {
        return -1;
    }

    return 0;
}