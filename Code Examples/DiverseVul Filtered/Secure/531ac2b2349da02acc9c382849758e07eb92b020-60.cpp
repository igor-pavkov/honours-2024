int manager_update_failed_units(Manager *m, Unit *u, bool failed) {
        unsigned size;
        int r;

        assert(m);
        assert(u->manager == m);

        size = set_size(m->failed_units);

        if (failed) {
                r = set_ensure_allocated(&m->failed_units, NULL);
                if (r < 0)
                        return log_oom();

                if (set_put(m->failed_units, u) < 0)
                        return log_oom();
        } else
                (void) set_remove(m->failed_units, u);

        if (set_size(m->failed_units) != size)
                bus_manager_send_change_signal(m);

        return 0;
}