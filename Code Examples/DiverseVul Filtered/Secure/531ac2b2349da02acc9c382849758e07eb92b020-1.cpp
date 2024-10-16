static void manager_clean_environment(Manager *m) {
        assert(m);

        /* Let's remove some environment variables that we
         * need ourselves to communicate with our clients */
        strv_env_unset_many(
                        m->environment,
                        "NOTIFY_SOCKET",
                        "MAINPID",
                        "MANAGERPID",
                        "LISTEN_PID",
                        "LISTEN_FDS",
                        "LISTEN_FDNAMES",
                        "WATCHDOG_PID",
                        "WATCHDOG_USEC",
                        NULL);
}