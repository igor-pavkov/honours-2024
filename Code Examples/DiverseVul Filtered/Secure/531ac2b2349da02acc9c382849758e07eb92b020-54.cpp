int manager_loop(Manager *m) {
        int r;

        RATELIMIT_DEFINE(rl, 1*USEC_PER_SEC, 50000);

        assert(m);
        m->exit_code = MANAGER_OK;

        /* Release the path cache */
        m->unit_path_cache = set_free_free(m->unit_path_cache);

        manager_check_finished(m);

        /* There might still be some zombies hanging around from
         * before we were exec()'ed. Let's reap them. */
        r = manager_dispatch_sigchld(m);
        if (r < 0)
                return r;

        while (m->exit_code == MANAGER_OK) {
                usec_t wait_usec;

                if (m->runtime_watchdog > 0 && m->runtime_watchdog != USEC_INFINITY && MANAGER_IS_SYSTEM(m))
                        watchdog_ping();

                if (!ratelimit_test(&rl)) {
                        /* Yay, something is going seriously wrong, pause a little */
                        log_warning("Looping too fast. Throttling execution a little.");
                        sleep(1);
                }

                if (manager_dispatch_load_queue(m) > 0)
                        continue;

                if (manager_dispatch_gc_queue(m) > 0)
                        continue;

                if (manager_dispatch_cleanup_queue(m) > 0)
                        continue;

                if (manager_dispatch_cgroup_queue(m) > 0)
                        continue;

                if (manager_dispatch_dbus_queue(m) > 0)
                        continue;

                /* Sleep for half the watchdog time */
                if (m->runtime_watchdog > 0 && m->runtime_watchdog != USEC_INFINITY && MANAGER_IS_SYSTEM(m)) {
                        wait_usec = m->runtime_watchdog / 2;
                        if (wait_usec <= 0)
                                wait_usec = 1;
                } else
                        wait_usec = USEC_INFINITY;

                r = sd_event_run(m->event, wait_usec);
                if (r < 0)
                        return log_error_errno(r, "Failed to run event loop: %m");
        }

        return m->exit_code;
}