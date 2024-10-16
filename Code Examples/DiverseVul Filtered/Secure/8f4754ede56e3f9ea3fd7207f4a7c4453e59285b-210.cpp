int bdrv_create_file(const char* filename, QEMUOptionParameter *options,
                     Error **errp)
{
    BlockDriver *drv;
    Error *local_err = NULL;
    int ret;

    drv = bdrv_find_protocol(filename, true);
    if (drv == NULL) {
        error_setg(errp, "Could not find protocol for file '%s'", filename);
        return -ENOENT;
    }

    ret = bdrv_create(drv, filename, options, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
    }
    return ret;
}