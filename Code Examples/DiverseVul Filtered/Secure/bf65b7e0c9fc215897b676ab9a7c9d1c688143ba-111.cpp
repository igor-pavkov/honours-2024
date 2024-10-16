int unit_prepare_exec(Unit *u) {
        int r;

        assert(u);

        /* Prepares everything so that we can fork of a process for this unit */

        (void) unit_realize_cgroup(u);

        if (u->reset_accounting) {
                (void) unit_reset_cpu_accounting(u);
                (void) unit_reset_ip_accounting(u);
                u->reset_accounting = false;
        }

        unit_export_state_files(u);

        r = unit_setup_exec_runtime(u);
        if (r < 0)
                return r;

        r = unit_setup_dynamic_creds(u);
        if (r < 0)
                return r;

        return 0;
}