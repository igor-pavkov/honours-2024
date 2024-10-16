void do_acpitable_option(const QemuOpts *opts)
{
#ifdef TARGET_I386
    Error *err = NULL;

    acpi_table_add(opts, &err);
    if (err) {
        error_report("Wrong acpi table provided: %s",
                     error_get_pretty(err));
        error_free(err);
        exit(1);
    }
#endif
}