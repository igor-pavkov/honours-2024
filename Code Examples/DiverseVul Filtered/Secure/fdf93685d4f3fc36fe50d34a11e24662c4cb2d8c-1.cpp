static int load_conf(const char *conf_filename)
{
    map_string_t *settings = new_map_string();
    if (conf_filename != NULL)
    {
        if (!load_conf_file(conf_filename, settings, false))
            error_msg("Can't open '%s'", conf_filename);
    }
    else
    {
        conf_filename = "abrt-action-save-package-data.conf";
        if (!load_abrt_conf_file(conf_filename, settings))
            error_msg("Can't load '%s'", conf_filename);
    }

    ParseCommon(settings, conf_filename);
    free_map_string(settings);

    load_gpg_keys();

    return 0;
}