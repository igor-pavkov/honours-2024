void manager_send_unit_audit(Manager *m, Unit *u, int type, bool success) {

#ifdef HAVE_AUDIT
        _cleanup_free_ char *p = NULL;
        const char *msg;
        int audit_fd, r;

        if (!MANAGER_IS_SYSTEM(m))
                return;

        audit_fd = get_audit_fd();
        if (audit_fd < 0)
                return;

        /* Don't generate audit events if the service was already
         * started and we're just deserializing */
        if (MANAGER_IS_RELOADING(m))
                return;

        if (u->type != UNIT_SERVICE)
                return;

        r = unit_name_to_prefix_and_instance(u->id, &p);
        if (r < 0) {
                log_error_errno(r, "Failed to extract prefix and instance of unit name: %m");
                return;
        }

        msg = strjoina("unit=", p);
        if (audit_log_user_comm_message(audit_fd, type, msg, "systemd", NULL, NULL, NULL, success) < 0) {
                if (errno == EPERM)
                        /* We aren't allowed to send audit messages?
                         * Then let's not retry again. */
                        close_audit_fd();
                else
                        log_warning_errno(errno, "Failed to send audit message: %m");
        }
#endif

}