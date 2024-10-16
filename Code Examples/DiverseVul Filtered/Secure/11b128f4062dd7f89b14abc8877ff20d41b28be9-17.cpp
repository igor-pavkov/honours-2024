static int qcow2_amend_options(BlockDriverState *bs,
                               QEMUOptionParameter *options)
{
    BDRVQcowState *s = bs->opaque;
    int old_version = s->qcow_version, new_version = old_version;
    uint64_t new_size = 0;
    const char *backing_file = NULL, *backing_format = NULL;
    bool lazy_refcounts = s->use_lazy_refcounts;
    int ret;
    int i;

    for (i = 0; options[i].name; i++)
    {
        if (!options[i].assigned) {
            /* only change explicitly defined options */
            continue;
        }

        if (!strcmp(options[i].name, "compat")) {
            if (!options[i].value.s) {
                /* preserve default */
            } else if (!strcmp(options[i].value.s, "0.10")) {
                new_version = 2;
            } else if (!strcmp(options[i].value.s, "1.1")) {
                new_version = 3;
            } else {
                fprintf(stderr, "Unknown compatibility level %s.\n",
                        options[i].value.s);
                return -EINVAL;
            }
        } else if (!strcmp(options[i].name, "preallocation")) {
            fprintf(stderr, "Cannot change preallocation mode.\n");
            return -ENOTSUP;
        } else if (!strcmp(options[i].name, "size")) {
            new_size = options[i].value.n;
        } else if (!strcmp(options[i].name, "backing_file")) {
            backing_file = options[i].value.s;
        } else if (!strcmp(options[i].name, "backing_fmt")) {
            backing_format = options[i].value.s;
        } else if (!strcmp(options[i].name, "encryption")) {
            if ((options[i].value.n != !!s->crypt_method)) {
                fprintf(stderr, "Changing the encryption flag is not "
                        "supported.\n");
                return -ENOTSUP;
            }
        } else if (!strcmp(options[i].name, "cluster_size")) {
            if (options[i].value.n != s->cluster_size) {
                fprintf(stderr, "Changing the cluster size is not "
                        "supported.\n");
                return -ENOTSUP;
            }
        } else if (!strcmp(options[i].name, "lazy_refcounts")) {
            lazy_refcounts = options[i].value.n;
        } else {
            /* if this assertion fails, this probably means a new option was
             * added without having it covered here */
            assert(false);
        }
    }

    if (new_version != old_version) {
        if (new_version > old_version) {
            /* Upgrade */
            s->qcow_version = new_version;
            ret = qcow2_update_header(bs);
            if (ret < 0) {
                s->qcow_version = old_version;
                return ret;
            }
        } else {
            ret = qcow2_downgrade(bs, new_version);
            if (ret < 0) {
                return ret;
            }
        }
    }

    if (backing_file || backing_format) {
        ret = qcow2_change_backing_file(bs, backing_file ?: bs->backing_file,
                                        backing_format ?: bs->backing_format);
        if (ret < 0) {
            return ret;
        }
    }

    if (s->use_lazy_refcounts != lazy_refcounts) {
        if (lazy_refcounts) {
            if (s->qcow_version < 3) {
                fprintf(stderr, "Lazy refcounts only supported with compatibility "
                        "level 1.1 and above (use compat=1.1 or greater)\n");
                return -EINVAL;
            }
            s->compatible_features |= QCOW2_COMPAT_LAZY_REFCOUNTS;
            ret = qcow2_update_header(bs);
            if (ret < 0) {
                s->compatible_features &= ~QCOW2_COMPAT_LAZY_REFCOUNTS;
                return ret;
            }
            s->use_lazy_refcounts = true;
        } else {
            /* make image clean first */
            ret = qcow2_mark_clean(bs);
            if (ret < 0) {
                return ret;
            }
            /* now disallow lazy refcounts */
            s->compatible_features &= ~QCOW2_COMPAT_LAZY_REFCOUNTS;
            ret = qcow2_update_header(bs);
            if (ret < 0) {
                s->compatible_features |= QCOW2_COMPAT_LAZY_REFCOUNTS;
                return ret;
            }
            s->use_lazy_refcounts = false;
        }
    }

    if (new_size) {
        ret = bdrv_truncate(bs, new_size);
        if (ret < 0) {
            return ret;
        }
    }

    return 0;
}