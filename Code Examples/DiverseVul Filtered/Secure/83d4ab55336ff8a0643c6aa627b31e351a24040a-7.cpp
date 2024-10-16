_public_ PAM_EXTERN int pam_sm_close_session(
                pam_handle_t *handle,
                int flags,
                int argc, const char **argv) {

        _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
        _cleanup_(sd_bus_flush_close_unrefp) sd_bus *bus = NULL;
        const void *existing = NULL;
        const char *id;
        int r;

        assert(handle);

        /* Only release session if it wasn't pre-existing when we
         * tried to create it */
        (void) pam_get_data(handle, "systemd.existing", &existing);

        id = pam_getenv(handle, "XDG_SESSION_ID");
        if (id && !existing) {

                /* Before we go and close the FIFO we need to tell
                 * logind that this is a clean session shutdown, so
                 * that it doesn't just go and slaughter us
                 * immediately after closing the fd */

                r = sd_bus_open_system(&bus);
                if (r < 0) {
                        pam_syslog(handle, LOG_ERR, "Failed to connect to system bus: %s", strerror(-r));
                        return PAM_SESSION_ERR;
                }

                r = sd_bus_call_method(bus,
                                       "org.freedesktop.login1",
                                       "/org/freedesktop/login1",
                                       "org.freedesktop.login1.Manager",
                                       "ReleaseSession",
                                       &error,
                                       NULL,
                                       "s",
                                       id);
                if (r < 0) {
                        pam_syslog(handle, LOG_ERR, "Failed to release session: %s", bus_error_message(&error, r));
                        return PAM_SESSION_ERR;
                }
        }

        /* Note that we are knowingly leaking the FIFO fd here. This
         * way, logind can watch us die. If we closed it here it would
         * not have any clue when that is completed. Given that one
         * cannot really have multiple PAM sessions open from the same
         * process this means we will leak one FD at max. */

        return PAM_SUCCESS;
}