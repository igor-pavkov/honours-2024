static int bdrv_is_whitelisted(BlockDriver *drv, bool read_only)
{
    static const char *whitelist_rw[] = {
        CONFIG_BDRV_RW_WHITELIST
    };
    static const char *whitelist_ro[] = {
        CONFIG_BDRV_RO_WHITELIST
    };
    const char **p;

    if (!whitelist_rw[0] && !whitelist_ro[0]) {
        return 1;               /* no whitelist, anything goes */
    }

    for (p = whitelist_rw; *p; p++) {
        if (!strcmp(drv->format_name, *p)) {
            return 1;
        }
    }
    if (read_only) {
        for (p = whitelist_ro; *p; p++) {
            if (!strcmp(drv->format_name, *p)) {
                return 1;
            }
        }
    }
    return 0;
}