void manager_recheck_journal(Manager *m) {
        Unit *u;

        assert(m);

        if (!MANAGER_IS_SYSTEM(m))
                return;

        u = manager_get_unit(m, SPECIAL_JOURNALD_SOCKET);
        if (u && SOCKET(u)->state != SOCKET_RUNNING) {
                log_close_journal();
                return;
        }

        u = manager_get_unit(m, SPECIAL_JOURNALD_SERVICE);
        if (u && SERVICE(u)->state != SERVICE_RUNNING) {
                log_close_journal();
                return;
        }

        /* Hmm, OK, so the socket is fully up and the service is up
         * too, then let's make use of the thing. */
        log_open();
}