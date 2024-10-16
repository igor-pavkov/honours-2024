static void invoke_sigchld_event(Manager *m, Unit *u, const siginfo_t *si) {
        uint64_t iteration;

        assert(m);
        assert(u);
        assert(si);

        sd_event_get_iteration(m->event, &iteration);

        log_unit_debug(u, "Child "PID_FMT" belongs to %s", si->si_pid, u->id);

        unit_unwatch_pid(u, si->si_pid);

        if (UNIT_VTABLE(u)->sigchld_event) {
                if (set_size(u->pids) <= 1 ||
                    iteration != u->sigchldgen ||
                    unit_main_pid(u) == si->si_pid ||
                    unit_control_pid(u) == si->si_pid) {
                        UNIT_VTABLE(u)->sigchld_event(u, si->si_pid, si->si_code, si->si_status);
                        u->sigchldgen = iteration;
                } else
                        log_debug("%s already issued a sigchld this iteration %" PRIu64 ", skipping. Pids still being watched %d", u->id, iteration, set_size(u->pids));
         }
}