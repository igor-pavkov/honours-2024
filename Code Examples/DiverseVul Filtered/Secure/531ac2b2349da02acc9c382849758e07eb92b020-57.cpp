static unsigned manager_dispatch_dbus_queue(Manager *m) {
        Job *j;
        Unit *u;
        unsigned n = 0;

        assert(m);

        if (m->dispatching_dbus_queue)
                return 0;

        m->dispatching_dbus_queue = true;

        while ((u = m->dbus_unit_queue)) {
                assert(u->in_dbus_queue);

                bus_unit_send_change_signal(u);
                n++;
        }

        while ((j = m->dbus_job_queue)) {
                assert(j->in_dbus_queue);

                bus_job_send_change_signal(j);
                n++;
        }

        m->dispatching_dbus_queue = false;

        if (m->send_reloading_done) {
                m->send_reloading_done = false;

                bus_manager_send_reloading(m, false);
        }

        if (m->queued_message)
                bus_send_queued_message(m);

        return n;
}