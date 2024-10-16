void unit_unlink_state_files(Unit *u) {
        const char *p;

        assert(u);

        if (!u->id)
                return;

        if (!MANAGER_IS_SYSTEM(u->manager))
                return;

        /* Undoes the effect of unit_export_state() */

        if (u->exported_invocation_id) {
                p = strjoina("/run/systemd/units/invocation:", u->id);
                (void) unlink(p);

                u->exported_invocation_id = false;
        }

        if (u->exported_log_level_max) {
                p = strjoina("/run/systemd/units/log-level-max:", u->id);
                (void) unlink(p);

                u->exported_log_level_max = false;
        }

        if (u->exported_log_extra_fields) {
                p = strjoina("/run/systemd/units/extra-fields:", u->id);
                (void) unlink(p);

                u->exported_log_extra_fields = false;
        }

        if (u->exported_log_rate_limit_interval) {
                p = strjoina("/run/systemd/units/log-rate-limit-interval:", u->id);
                (void) unlink(p);

                u->exported_log_rate_limit_interval = false;
        }

        if (u->exported_log_rate_limit_burst) {
                p = strjoina("/run/systemd/units/log-rate-limit-burst:", u->id);
                (void) unlink(p);

                u->exported_log_rate_limit_burst = false;
        }
}