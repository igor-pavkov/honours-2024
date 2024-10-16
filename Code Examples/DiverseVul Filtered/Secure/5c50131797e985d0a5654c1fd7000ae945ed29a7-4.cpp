gboolean session_info_session_is_locked(struct session_info *info)
{
    gboolean locked;

    g_return_val_if_fail (info != NULL, FALSE);

    /* Not every system does emit Lock and Unlock signals (for instance, such
     * is the case for RHEL6) but most of the systems seems to emit the
     * IdleHintChanged. So use the IdleHint value.
     * systemd-login uses locked-hint which is not implemented in ConsoleKit,
     * see https://github.com/ConsoleKit2/ConsoleKit2/issues/89 */
    si_dbus_read_signals(info);
    locked = info->session_idle_hint;
    if (info->verbose) {
        syslog(LOG_DEBUG, "(console-kit) session is locked: %s",
               locked ? "yes" : "no");
    }
    return locked;
}