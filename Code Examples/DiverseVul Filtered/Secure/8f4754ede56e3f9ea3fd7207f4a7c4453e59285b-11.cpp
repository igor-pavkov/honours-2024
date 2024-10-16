BlockDriver *bdrv_find_whitelisted_format(const char *format_name,
                                          bool read_only)
{
    BlockDriver *drv = bdrv_find_format(format_name);
    return drv && bdrv_is_whitelisted(drv, read_only) ? drv : NULL;
}