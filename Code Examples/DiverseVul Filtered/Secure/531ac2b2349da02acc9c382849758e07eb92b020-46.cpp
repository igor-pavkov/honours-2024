int manager_startup(Manager *m, FILE *serialization, FDSet *fds) {
        int r, q;

        assert(m);

        r = lookup_paths_init(&m->lookup_paths, m->unit_file_scope, 0, NULL);
        if (r < 0)
                return r;

        /* Make sure the transient directory always exists, so that it remains in the search path */
        r = mkdir_p_label(m->lookup_paths.transient, 0755);
        if (r < 0)
                return r;

        dual_timestamp_get(&m->generators_start_timestamp);
        r = manager_run_generators(m);
        dual_timestamp_get(&m->generators_finish_timestamp);
        if (r < 0)
                return r;

        lookup_paths_reduce(&m->lookup_paths);
        manager_build_unit_path_cache(m);

        /* If we will deserialize make sure that during enumeration
         * this is already known, so we increase the counter here
         * already */
        if (serialization)
                m->n_reloading++;

        /* First, enumerate what we can from all config files */
        dual_timestamp_get(&m->units_load_start_timestamp);
        manager_enumerate(m);
        dual_timestamp_get(&m->units_load_finish_timestamp);

        /* Second, deserialize if there is something to deserialize */
        if (serialization)
                r = manager_deserialize(m, serialization, fds);

        /* Any fds left? Find some unit which wants them. This is
         * useful to allow container managers to pass some file
         * descriptors to us pre-initialized. This enables
         * socket-based activation of entire containers. */
        manager_distribute_fds(m, fds);

        /* We might have deserialized the notify fd, but if we didn't
         * then let's create the bus now */
        q = manager_setup_notify(m);
        if (q < 0 && r == 0)
                r = q;

        q = manager_setup_cgroups_agent(m);
        if (q < 0 && r == 0)
                r = q;

        q = manager_setup_user_lookup_fd(m);
        if (q < 0 && r == 0)
                r = q;

        /* Let's connect to the bus now. */
        (void) manager_connect_bus(m, !!serialization);

        (void) bus_track_coldplug(m, &m->subscribed, false, m->deserialized_subscribed);
        m->deserialized_subscribed = strv_free(m->deserialized_subscribed);

        /* Third, fire things up! */
        manager_coldplug(m);

        /* Release any dynamic users no longer referenced */
        dynamic_user_vacuum(m, true);

        /* Release any references to UIDs/GIDs no longer referenced, and destroy any IPC owned by them */
        manager_vacuum_uid_refs(m);
        manager_vacuum_gid_refs(m);

        if (serialization) {
                assert(m->n_reloading > 0);
                m->n_reloading--;

                /* Let's wait for the UnitNew/JobNew messages being
                 * sent, before we notify that the reload is
                 * finished */
                m->send_reloading_done = true;
        }

        return r;
}