static void unit_emit_audit_stop(Unit *u, UnitActiveState state) {
        assert(u);

        if (u->type != UNIT_SERVICE)
                return;

        if (u->in_audit) {
                /* Write audit record if we have just finished shutting down */
                manager_send_unit_audit(u->manager, u, AUDIT_SERVICE_STOP, state == UNIT_INACTIVE);
                u->in_audit = false;
        } else {
                /* Hmm, if there was no start record written write it now, so that we always have a nice pair */
                manager_send_unit_audit(u->manager, u, AUDIT_SERVICE_START, state == UNIT_INACTIVE);

                if (state == UNIT_INACTIVE)
                        manager_send_unit_audit(u->manager, u, AUDIT_SERVICE_STOP, true);
        }
}