void bdrv_img_create(const char *filename, const char *fmt,
                     const char *base_filename, const char *base_fmt,
                     char *options, uint64_t img_size, int flags,
                     Error **errp, bool quiet)
{
    QEMUOptionParameter *param = NULL, *create_options = NULL;
    QEMUOptionParameter *backing_fmt, *backing_file, *size;
    BlockDriver *drv, *proto_drv;
    BlockDriver *backing_drv = NULL;
    Error *local_err = NULL;
    int ret = 0;

    /* Find driver and parse its options */
    drv = bdrv_find_format(fmt);
    if (!drv) {
        error_setg(errp, "Unknown file format '%s'", fmt);
        return;
    }

    proto_drv = bdrv_find_protocol(filename, true);
    if (!proto_drv) {
        error_setg(errp, "Unknown protocol '%s'", filename);
        return;
    }

    create_options = append_option_parameters(create_options,
                                              drv->create_options);
    create_options = append_option_parameters(create_options,
                                              proto_drv->create_options);

    /* Create parameter list with default values */
    param = parse_option_parameters("", create_options, param);

    set_option_parameter_int(param, BLOCK_OPT_SIZE, img_size);

    /* Parse -o options */
    if (options) {
        param = parse_option_parameters(options, create_options, param);
        if (param == NULL) {
            error_setg(errp, "Invalid options for file format '%s'.", fmt);
            goto out;
        }
    }

    if (base_filename) {
        if (set_option_parameter(param, BLOCK_OPT_BACKING_FILE,
                                 base_filename)) {
            error_setg(errp, "Backing file not supported for file format '%s'",
                       fmt);
            goto out;
        }
    }

    if (base_fmt) {
        if (set_option_parameter(param, BLOCK_OPT_BACKING_FMT, base_fmt)) {
            error_setg(errp, "Backing file format not supported for file "
                             "format '%s'", fmt);
            goto out;
        }
    }

    backing_file = get_option_parameter(param, BLOCK_OPT_BACKING_FILE);
    if (backing_file && backing_file->value.s) {
        if (!strcmp(filename, backing_file->value.s)) {
            error_setg(errp, "Error: Trying to create an image with the "
                             "same filename as the backing file");
            goto out;
        }
    }

    backing_fmt = get_option_parameter(param, BLOCK_OPT_BACKING_FMT);
    if (backing_fmt && backing_fmt->value.s) {
        backing_drv = bdrv_find_format(backing_fmt->value.s);
        if (!backing_drv) {
            error_setg(errp, "Unknown backing file format '%s'",
                       backing_fmt->value.s);
            goto out;
        }
    }

    // The size for the image must always be specified, with one exception:
    // If we are using a backing file, we can obtain the size from there
    size = get_option_parameter(param, BLOCK_OPT_SIZE);
    if (size && size->value.n == -1) {
        if (backing_file && backing_file->value.s) {
            BlockDriverState *bs;
            uint64_t size;
            char buf[32];
            int back_flags;

            /* backing files always opened read-only */
            back_flags =
                flags & ~(BDRV_O_RDWR | BDRV_O_SNAPSHOT | BDRV_O_NO_BACKING);

            bs = NULL;
            ret = bdrv_open(&bs, backing_file->value.s, NULL, NULL, back_flags,
                            backing_drv, &local_err);
            if (ret < 0) {
                error_setg_errno(errp, -ret, "Could not open '%s': %s",
                                 backing_file->value.s,
                                 error_get_pretty(local_err));
                error_free(local_err);
                local_err = NULL;
                goto out;
            }
            bdrv_get_geometry(bs, &size);
            size *= 512;

            snprintf(buf, sizeof(buf), "%" PRId64, size);
            set_option_parameter(param, BLOCK_OPT_SIZE, buf);

            bdrv_unref(bs);
        } else {
            error_setg(errp, "Image creation needs a size parameter");
            goto out;
        }
    }

    if (!quiet) {
        printf("Formatting '%s', fmt=%s ", filename, fmt);
        print_option_parameters(param);
        puts("");
    }
    ret = bdrv_create(drv, filename, param, &local_err);
    if (ret == -EFBIG) {
        /* This is generally a better message than whatever the driver would
         * deliver (especially because of the cluster_size_hint), since that
         * is most probably not much different from "image too large". */
        const char *cluster_size_hint = "";
        if (get_option_parameter(create_options, BLOCK_OPT_CLUSTER_SIZE)) {
            cluster_size_hint = " (try using a larger cluster size)";
        }
        error_setg(errp, "The image size is too large for file format '%s'"
                   "%s", fmt, cluster_size_hint);
        error_free(local_err);
        local_err = NULL;
    }

out:
    free_option_parameters(create_options);
    free_option_parameters(param);

    if (local_err) {
        error_propagate(errp, local_err);
    }
}