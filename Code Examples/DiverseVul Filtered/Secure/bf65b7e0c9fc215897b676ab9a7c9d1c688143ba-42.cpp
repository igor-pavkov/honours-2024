int unit_warn_leftover_processes(Unit *u) {
        assert(u);

        (void) unit_pick_cgroup_path(u);

        if (!u->cgroup_path)
                return 0;

        return cg_kill_recursive(SYSTEMD_CGROUP_CONTROLLER, u->cgroup_path, 0, 0, NULL, log_leftover, u);
}