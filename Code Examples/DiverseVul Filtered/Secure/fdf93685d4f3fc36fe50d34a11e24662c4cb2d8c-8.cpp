static void load_gpg_keys(void)
{
    map_string_t *settings = new_map_string();
    if (!load_abrt_conf_file(GPG_CONF, settings))
    {
        error_msg("Can't load '%s'", GPG_CONF);
        return;
    }

    const char *gpg_keys_dir = get_map_string_item_or_NULL(settings, "GPGKeysDir");
    if (strcmp(gpg_keys_dir, "") != 0)
    {
        log_debug("Reading gpg keys from '%s'", gpg_keys_dir);
        GList *gpg_files = get_file_list(gpg_keys_dir, NULL /* we don't care about the file ext */);
        GList *tmp_gpp_files = gpg_files;
        while (tmp_gpp_files)
        {
            log_debug("Loading gpg key '%s'", fo_get_fullpath((file_obj_t *)tmp_gpp_files->data));
            settings_setOpenGPGPublicKeys = g_list_append(settings_setOpenGPGPublicKeys, xstrdup(fo_get_fullpath((file_obj_t *)(tmp_gpp_files->data)) ));
            tmp_gpp_files = g_list_next(tmp_gpp_files);
        }

        g_list_free_full(gpg_files, (GDestroyNotify)free_file_obj);
    }
}