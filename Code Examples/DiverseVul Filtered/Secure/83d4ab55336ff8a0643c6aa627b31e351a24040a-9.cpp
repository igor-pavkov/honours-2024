static int export_legacy_dbus_address(
                pam_handle_t *handle,
                uid_t uid,
                const char *runtime) {

        const char *s;
        _cleanup_free_ char *t = NULL;
        int r = PAM_BUF_ERR;

        /* We need to export $DBUS_SESSION_BUS_ADDRESS because various applications will not connect
         * correctly to the bus without it. This setting matches what dbus.socket does for the user
         * session using 'systemctl --user set-environment'. We want to have the same configuration
         * in processes started from the PAM session.
         *
         * The setting of the address is guarded by the access() check because it is also possible to compile
         * dbus without --enable-user-session, in which case this socket is not used, and
         * $DBUS_SESSION_BUS_ADDRESS should not be set. An alternative approach would to not do the access()
         * check here, and let applications try on their own, by using "unix:path=%s/bus;autolaunch:". But we
         * expect the socket to be present by the time we do this check, so we can just as well check once
         * here. */

        s = strjoina(runtime, "/bus");
        if (access(s, F_OK) < 0)
                return PAM_SUCCESS;

        if (asprintf(&t, DEFAULT_USER_BUS_ADDRESS_FMT, runtime) < 0)
                goto error;

        r = pam_misc_setenv(handle, "DBUS_SESSION_BUS_ADDRESS", t, 0);
        if (r != PAM_SUCCESS)
                goto error;

        return PAM_SUCCESS;

error:
        pam_syslog(handle, LOG_ERR, "Failed to set bus variable.");
        return r;
}