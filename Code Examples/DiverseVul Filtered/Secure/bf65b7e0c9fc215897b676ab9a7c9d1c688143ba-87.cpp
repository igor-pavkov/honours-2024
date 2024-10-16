void unit_status_printf(Unit *u, const char *status, const char *unit_status_msg_format) {
        const char *d;

        d = unit_description(u);
        if (log_get_show_color())
                d = strjoina(ANSI_HIGHLIGHT, d, ANSI_NORMAL);

        DISABLE_WARNING_FORMAT_NONLITERAL;
        manager_status_printf(u->manager, STATUS_TYPE_NORMAL, status, unit_status_msg_format, d);
        REENABLE_WARNING;
}