static int bdrv_file_open(BlockDriverState *bs, const char *filename,
                          QDict **options, int flags, Error **errp)
{
    BlockDriver *drv;
    const char *drvname;
    bool allow_protocol_prefix = false;
    Error *local_err = NULL;
    int ret;

    /* Fetch the file name from the options QDict if necessary */
    if (!filename) {
        filename = qdict_get_try_str(*options, "filename");
    } else if (filename && !qdict_haskey(*options, "filename")) {
        qdict_put(*options, "filename", qstring_from_str(filename));
        allow_protocol_prefix = true;
    } else {
        error_setg(errp, "Can't specify 'file' and 'filename' options at the "
                   "same time");
        ret = -EINVAL;
        goto fail;
    }

    /* Find the right block driver */
    drvname = qdict_get_try_str(*options, "driver");
    if (drvname) {
        drv = bdrv_find_format(drvname);
        if (!drv) {
            error_setg(errp, "Unknown driver '%s'", drvname);
        }
        qdict_del(*options, "driver");
    } else if (filename) {
        drv = bdrv_find_protocol(filename, allow_protocol_prefix);
        if (!drv) {
            error_setg(errp, "Unknown protocol");
        }
    } else {
        error_setg(errp, "Must specify either driver or file");
        drv = NULL;
    }

    if (!drv) {
        /* errp has been set already */
        ret = -ENOENT;
        goto fail;
    }

    /* Parse the filename and open it */
    if (drv->bdrv_parse_filename && filename) {
        drv->bdrv_parse_filename(filename, *options, &local_err);
        if (local_err) {
            error_propagate(errp, local_err);
            ret = -EINVAL;
            goto fail;
        }

        if (!drv->bdrv_needs_filename) {
            qdict_del(*options, "filename");
        } else {
            filename = qdict_get_str(*options, "filename");
        }
    }

    if (!drv->bdrv_file_open) {
        ret = bdrv_open(&bs, filename, NULL, *options, flags, drv, &local_err);
        *options = NULL;
    } else {
        ret = bdrv_open_common(bs, NULL, *options, flags, drv, &local_err);
    }
    if (ret < 0) {
        error_propagate(errp, local_err);
        goto fail;
    }

    bs->growable = 1;
    return 0;

fail:
    return ret;
}