static int local_parse_opts(QemuOpts *opts, struct FsDriverEntry *fse)
{
    const char *sec_model = qemu_opt_get(opts, "security_model");
    const char *path = qemu_opt_get(opts, "path");
    Error *err = NULL;

    if (!sec_model) {
        error_report("Security model not specified, local fs needs security model");
        error_printf("valid options are:"
                     "\tsecurity_model=[passthrough|mapped-xattr|mapped-file|none]\n");
        return -1;
    }

    if (!strcmp(sec_model, "passthrough")) {
        fse->export_flags |= V9FS_SM_PASSTHROUGH;
    } else if (!strcmp(sec_model, "mapped") ||
               !strcmp(sec_model, "mapped-xattr")) {
        fse->export_flags |= V9FS_SM_MAPPED;
    } else if (!strcmp(sec_model, "none")) {
        fse->export_flags |= V9FS_SM_NONE;
    } else if (!strcmp(sec_model, "mapped-file")) {
        fse->export_flags |= V9FS_SM_MAPPED_FILE;
    } else {
        error_report("Invalid security model %s specified", sec_model);
        error_printf("valid options are:"
                     "\t[passthrough|mapped-xattr|mapped-file|none]\n");
        return -1;
    }

    if (!path) {
        error_report("fsdev: No path specified");
        return -1;
    }

    fsdev_throttle_parse_opts(opts, &fse->fst, &err);
    if (err) {
        error_reportf_err(err, "Throttle configuration is not valid: ");
        return -1;
    }

    fse->path = g_strdup(path);

    return 0;
}