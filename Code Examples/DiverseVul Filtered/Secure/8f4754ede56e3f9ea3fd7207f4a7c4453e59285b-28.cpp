int bdrv_open(BlockDriverState **pbs, const char *filename,
              const char *reference, QDict *options, int flags,
              BlockDriver *drv, Error **errp)
{
    int ret;
    /* TODO: extra byte is a hack to ensure MAX_PATH space on Windows. */
    char tmp_filename[PATH_MAX + 1];
    BlockDriverState *file = NULL, *bs;
    const char *drvname;
    Error *local_err = NULL;

    assert(pbs);

    if (reference) {
        bool options_non_empty = options ? qdict_size(options) : false;
        QDECREF(options);

        if (*pbs) {
            error_setg(errp, "Cannot reuse an existing BDS when referencing "
                       "another block device");
            return -EINVAL;
        }

        if (filename || options_non_empty) {
            error_setg(errp, "Cannot reference an existing block device with "
                       "additional options or a new filename");
            return -EINVAL;
        }

        bs = bdrv_lookup_bs(reference, reference, errp);
        if (!bs) {
            return -ENODEV;
        }
        bdrv_ref(bs);
        *pbs = bs;
        return 0;
    }

    if (*pbs) {
        bs = *pbs;
    } else {
        bs = bdrv_new("");
    }

    /* NULL means an empty set of options */
    if (options == NULL) {
        options = qdict_new();
    }

    bs->options = options;
    options = qdict_clone_shallow(options);

    if (flags & BDRV_O_PROTOCOL) {
        assert(!drv);
        ret = bdrv_file_open(bs, filename, &options, flags & ~BDRV_O_PROTOCOL,
                             &local_err);
        if (!ret) {
            drv = bs->drv;
            goto done;
        } else if (bs->drv) {
            goto close_and_fail;
        } else {
            goto fail;
        }
    }

    /* For snapshot=on, create a temporary qcow2 overlay */
    if (flags & BDRV_O_SNAPSHOT) {
        BlockDriverState *bs1;
        int64_t total_size;
        BlockDriver *bdrv_qcow2;
        QEMUOptionParameter *create_options;
        QDict *snapshot_options;

        /* if snapshot, we create a temporary backing file and open it
           instead of opening 'filename' directly */

        /* Get the required size from the image */
        QINCREF(options);
        bs1 = NULL;
        ret = bdrv_open(&bs1, filename, NULL, options, BDRV_O_NO_BACKING,
                        drv, &local_err);
        if (ret < 0) {
            goto fail;
        }
        total_size = bdrv_getlength(bs1) & BDRV_SECTOR_MASK;

        bdrv_unref(bs1);

        /* Create the temporary image */
        ret = get_tmp_filename(tmp_filename, sizeof(tmp_filename));
        if (ret < 0) {
            error_setg_errno(errp, -ret, "Could not get temporary filename");
            goto fail;
        }

        bdrv_qcow2 = bdrv_find_format("qcow2");
        create_options = parse_option_parameters("", bdrv_qcow2->create_options,
                                                 NULL);

        set_option_parameter_int(create_options, BLOCK_OPT_SIZE, total_size);

        ret = bdrv_create(bdrv_qcow2, tmp_filename, create_options, &local_err);
        free_option_parameters(create_options);
        if (ret < 0) {
            error_setg_errno(errp, -ret, "Could not create temporary overlay "
                             "'%s': %s", tmp_filename,
                             error_get_pretty(local_err));
            error_free(local_err);
            local_err = NULL;
            goto fail;
        }

        /* Prepare a new options QDict for the temporary file, where user
         * options refer to the backing file */
        if (filename) {
            qdict_put(options, "file.filename", qstring_from_str(filename));
        }
        if (drv) {
            qdict_put(options, "driver", qstring_from_str(drv->format_name));
        }

        snapshot_options = qdict_new();
        qdict_put(snapshot_options, "backing", options);
        qdict_flatten(snapshot_options);

        bs->options = snapshot_options;
        options = qdict_clone_shallow(bs->options);

        filename = tmp_filename;
        drv = bdrv_qcow2;
        bs->is_temporary = 1;
    }

    /* Open image file without format layer */
    if (flags & BDRV_O_RDWR) {
        flags |= BDRV_O_ALLOW_RDWR;
    }

    assert(file == NULL);
    ret = bdrv_open_image(&file, filename, options, "file",
                          bdrv_open_flags(bs, flags | BDRV_O_UNMAP) |
                          BDRV_O_PROTOCOL, true, &local_err);
    if (ret < 0) {
        goto unlink_and_fail;
    }

    /* Find the right image format driver */
    drvname = qdict_get_try_str(options, "driver");
    if (drvname) {
        drv = bdrv_find_format(drvname);
        qdict_del(options, "driver");
        if (!drv) {
            error_setg(errp, "Invalid driver: '%s'", drvname);
            ret = -EINVAL;
            goto unlink_and_fail;
        }
    }

    if (!drv) {
        if (file) {
            ret = find_image_format(file, filename, &drv, &local_err);
        } else {
            error_setg(errp, "Must specify either driver or file");
            ret = -EINVAL;
            goto unlink_and_fail;
        }
    }

    if (!drv) {
        goto unlink_and_fail;
    }

    /* Open the image */
    ret = bdrv_open_common(bs, file, options, flags, drv, &local_err);
    if (ret < 0) {
        goto unlink_and_fail;
    }

    if (file && (bs->file != file)) {
        bdrv_unref(file);
        file = NULL;
    }

    /* If there is a backing file, use it */
    if ((flags & BDRV_O_NO_BACKING) == 0) {
        QDict *backing_options;

        qdict_extract_subqdict(options, &backing_options, "backing.");
        ret = bdrv_open_backing_file(bs, backing_options, &local_err);
        if (ret < 0) {
            goto close_and_fail;
        }
    }

done:
    /* Check if any unknown options were used */
    if (options && (qdict_size(options) != 0)) {
        const QDictEntry *entry = qdict_first(options);
        if (flags & BDRV_O_PROTOCOL) {
            error_setg(errp, "Block protocol '%s' doesn't support the option "
                       "'%s'", drv->format_name, entry->key);
        } else {
            error_setg(errp, "Block format '%s' used by device '%s' doesn't "
                       "support the option '%s'", drv->format_name,
                       bs->device_name, entry->key);
        }

        ret = -EINVAL;
        goto close_and_fail;
    }

    if (!bdrv_key_required(bs)) {
        bdrv_dev_change_media_cb(bs, true);
    } else if (!runstate_check(RUN_STATE_PRELAUNCH)
               && !runstate_check(RUN_STATE_INMIGRATE)
               && !runstate_check(RUN_STATE_PAUSED)) { /* HACK */
        error_setg(errp,
                   "Guest must be stopped for opening of encrypted image");
        ret = -EBUSY;
        goto close_and_fail;
    }

    QDECREF(options);
    *pbs = bs;
    return 0;

unlink_and_fail:
    if (file != NULL) {
        bdrv_unref(file);
    }
    if (bs->is_temporary) {
        unlink(filename);
    }
fail:
    QDECREF(bs->options);
    QDECREF(options);
    bs->options = NULL;
    if (!*pbs) {
        /* If *pbs is NULL, a new BDS has been created in this function and
           needs to be freed now. Otherwise, it does not need to be closed,
           since it has not really been opened yet. */
        bdrv_unref(bs);
    }
    if (local_err) {
        error_propagate(errp, local_err);
    }
    return ret;

close_and_fail:
    /* See fail path, but now the BDS has to be always closed */
    if (*pbs) {
        bdrv_close(bs);
    } else {
        bdrv_unref(bs);
    }
    QDECREF(options);
    if (local_err) {
        error_propagate(errp, local_err);
    }
    return ret;
}