void do_smbios_option(QemuOpts *opts)
{
#ifdef TARGET_I386
    smbios_entry_add(opts);
#endif
}