gboolean session_info_is_user(struct session_info *si)
{
    gchar *class = NULL;
    gboolean ret;

    g_return_val_if_fail (si != NULL, TRUE);
    g_return_val_if_fail (si->session != NULL, TRUE);

    if (sd_session_get_class(si->session, &class) != 0) {
        syslog(LOG_WARNING, "Unable to get class from session: %s",
               si->session);
        return TRUE;
    }

    if (si->verbose)
        syslog(LOG_DEBUG, "(systemd-login) class for %s is %s",
               si->session, class);

    ret = (g_strcmp0(class, "user") == 0);
    g_free(class);

    return ret;
}