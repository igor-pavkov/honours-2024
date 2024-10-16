static void unit_emit_audit_start(Unit *u) {
        assert(u);

        if (u->type != UNIT_SERVICE)
                return;

        /* Write audit record if we have just finished starting up */
        manager_send_unit_audit(u->manager, u, AUDIT_SERVICE_START, true);
        u->in_audit = true;
}