int main(int argc, char **argv)
{
    /* I18n */
    setlocale(LC_ALL, "");
#if ENABLE_NLS
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);
#endif

    abrt_init(argv);

    const char *dump_dir_name = ".";
    const char *conf_filename = NULL;

    /* Can't keep these strings/structs static: _() doesn't support that */
    const char *program_usage_string = _(
        "& [-v] [-c CONFFILE] -d DIR\n"
        "\n"
        "Query package database and save package and component name"
    );
    enum {
        OPT_v = 1 << 0,
        OPT_d = 1 << 1,
        OPT_c = 1 << 2,
    };
    /* Keep enum above and order of options below in sync! */
    struct options program_options[] = {
        OPT__VERBOSE(&g_verbose),
        OPT_STRING('d', NULL, &dump_dir_name, "DIR"     , _("Problem directory")),
        OPT_STRING('c', NULL, &conf_filename, "CONFFILE", _("Configuration file")),
        OPT_END()
    };
    /*unsigned opts =*/ parse_opts(argc, argv, program_options, program_usage_string);

    export_abrt_envvars(0);

    log_notice("Loading settings");
    if (load_conf(conf_filename) != 0)
        return 1; /* syntax error (logged already by load_conf) */

    log_notice("Initializing rpm library");
    rpm_init();

    GList *li;
    for (li = settings_setOpenGPGPublicKeys; li != NULL; li = g_list_next(li))
    {
        log_notice("Loading GPG key '%s'", (char*)li->data);
        rpm_load_gpgkey((char*)li->data);
    }

    int r = SavePackageDescriptionToDebugDump(dump_dir_name);

    /* Close RPM database */
    rpm_destroy();

    return r;
}