static void iscsi_parse_filename(const char *filename, QDict *options,
                                 Error **errp)
{
    struct iscsi_url *iscsi_url;
    const char *transport_name;
    char *lun_str;

    iscsi_url = iscsi_parse_full_url(NULL, filename);
    if (iscsi_url == NULL) {
        error_setg(errp, "Failed to parse URL : %s", filename);
        return;
    }

#if LIBISCSI_API_VERSION >= (20160603)
    switch (iscsi_url->transport) {
    case TCP_TRANSPORT:
        transport_name = "tcp";
        break;
    case ISER_TRANSPORT:
        transport_name = "iser";
        break;
    default:
        error_setg(errp, "Unknown transport type (%d)",
                   iscsi_url->transport);
        return;
    }
#else
    transport_name = "tcp";
#endif

    qdict_set_default_str(options, "transport", transport_name);
    qdict_set_default_str(options, "portal", iscsi_url->portal);
    qdict_set_default_str(options, "target", iscsi_url->target);

    lun_str = g_strdup_printf("%d", iscsi_url->lun);
    qdict_set_default_str(options, "lun", lun_str);
    g_free(lun_str);

    /* User/password from -iscsi take precedence over those from the URL */
    iscsi_parse_iscsi_option(iscsi_url->target, options);

    if (iscsi_url->user[0] != '\0') {
        qdict_set_default_str(options, "user", iscsi_url->user);
        qdict_set_default_str(options, "password", iscsi_url->passwd);
    }

    iscsi_destroy_url(iscsi_url);
}