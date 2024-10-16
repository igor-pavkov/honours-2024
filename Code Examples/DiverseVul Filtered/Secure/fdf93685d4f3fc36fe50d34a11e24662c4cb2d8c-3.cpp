static void ParseCommon(map_string_t *settings, const char *conf_filename)
{
    const char *value;

    value = get_map_string_item_or_NULL(settings, "OpenGPGCheck");
    if (value)
    {
        settings_bOpenGPGCheck = string_to_bool(value);
        remove_map_string_item(settings, "OpenGPGCheck");
    }

    value = get_map_string_item_or_NULL(settings, "BlackList");
    if (value)
    {
        settings_setBlackListedPkgs = parse_list(value);
        remove_map_string_item(settings, "BlackList");
    }

    value = get_map_string_item_or_NULL(settings, "BlackListedPaths");
    if (value)
    {
        settings_setBlackListedPaths = parse_list(value);
        remove_map_string_item(settings, "BlackListedPaths");
    }

    value = get_map_string_item_or_NULL(settings, "ProcessUnpackaged");
    if (value)
    {
        settings_bProcessUnpackaged = string_to_bool(value);
        remove_map_string_item(settings, "ProcessUnpackaged");
    }

    value = get_map_string_item_or_NULL(settings, "Interpreters");
    if (value)
    {
        settings_Interpreters = parse_list(value);
        remove_map_string_item(settings, "Interpreters");
    }

    map_string_iter_t iter;
    const char *name;
    /*char *value; - already declared */
    init_map_string_iter(&iter, settings);
    while (next_map_string_iter(&iter, &name, &value))
    {
        error_msg("Unrecognized variable '%s' in '%s'", name, conf_filename);
    }
}