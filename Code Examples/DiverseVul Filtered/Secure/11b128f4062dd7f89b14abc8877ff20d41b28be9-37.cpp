static int qcow2_create(const char *filename, QEMUOptionParameter *options,
                        Error **errp)
{
    const char *backing_file = NULL;
    const char *backing_fmt = NULL;
    uint64_t sectors = 0;
    int flags = 0;
    size_t cluster_size = DEFAULT_CLUSTER_SIZE;
    int prealloc = 0;
    int version = 3;
    Error *local_err = NULL;
    int ret;

    /* Read out options */
    while (options && options->name) {
        if (!strcmp(options->name, BLOCK_OPT_SIZE)) {
            sectors = options->value.n / 512;
        } else if (!strcmp(options->name, BLOCK_OPT_BACKING_FILE)) {
            backing_file = options->value.s;
        } else if (!strcmp(options->name, BLOCK_OPT_BACKING_FMT)) {
            backing_fmt = options->value.s;
        } else if (!strcmp(options->name, BLOCK_OPT_ENCRYPT)) {
            flags |= options->value.n ? BLOCK_FLAG_ENCRYPT : 0;
        } else if (!strcmp(options->name, BLOCK_OPT_CLUSTER_SIZE)) {
            if (options->value.n) {
                cluster_size = options->value.n;
            }
        } else if (!strcmp(options->name, BLOCK_OPT_PREALLOC)) {
            if (!options->value.s || !strcmp(options->value.s, "off")) {
                prealloc = 0;
            } else if (!strcmp(options->value.s, "metadata")) {
                prealloc = 1;
            } else {
                error_setg(errp, "Invalid preallocation mode: '%s'",
                           options->value.s);
                return -EINVAL;
            }
        } else if (!strcmp(options->name, BLOCK_OPT_COMPAT_LEVEL)) {
            if (!options->value.s) {
                /* keep the default */
            } else if (!strcmp(options->value.s, "0.10")) {
                version = 2;
            } else if (!strcmp(options->value.s, "1.1")) {
                version = 3;
            } else {
                error_setg(errp, "Invalid compatibility level: '%s'",
                           options->value.s);
                return -EINVAL;
            }
        } else if (!strcmp(options->name, BLOCK_OPT_LAZY_REFCOUNTS)) {
            flags |= options->value.n ? BLOCK_FLAG_LAZY_REFCOUNTS : 0;
        }
        options++;
    }

    if (backing_file && prealloc) {
        error_setg(errp, "Backing file and preallocation cannot be used at "
                   "the same time");
        return -EINVAL;
    }

    if (version < 3 && (flags & BLOCK_FLAG_LAZY_REFCOUNTS)) {
        error_setg(errp, "Lazy refcounts only supported with compatibility "
                   "level 1.1 and above (use compat=1.1 or greater)");
        return -EINVAL;
    }

    ret = qcow2_create2(filename, sectors, backing_file, backing_fmt, flags,
                        cluster_size, prealloc, options, version, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
    }
    return ret;
}