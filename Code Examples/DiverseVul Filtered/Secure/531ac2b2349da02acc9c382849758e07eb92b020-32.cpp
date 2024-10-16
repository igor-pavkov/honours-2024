void manager_status_printf(Manager *m, StatusType type, const char *status, const char *format, ...) {
        va_list ap;

        /* If m is NULL, assume we're after shutdown and let the messages through. */

        if (m && !manager_get_show_status(m, type))
                return;

        /* XXX We should totally drop the check for ephemeral here
         * and thus effectively make 'Type=idle' pointless. */
        if (type == STATUS_TYPE_EPHEMERAL && m && m->n_on_console > 0)
                return;

        va_start(ap, format);
        status_vprintf(status, true, type == STATUS_TYPE_EPHEMERAL, format, ap);
        va_end(ap);
}