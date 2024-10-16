Manager* manager_free(Manager *m) {
        UnitType c;
        int i;

        if (!m)
                return NULL;

        manager_clear_jobs_and_units(m);

        for (c = 0; c < _UNIT_TYPE_MAX; c++)
                if (unit_vtable[c]->shutdown)
                        unit_vtable[c]->shutdown(m);

        /* If we reexecute ourselves, we keep the root cgroup
         * around */
        manager_shutdown_cgroup(m, m->exit_code != MANAGER_REEXECUTE);

        lookup_paths_flush_generator(&m->lookup_paths);

        bus_done(m);

        dynamic_user_vacuum(m, false);
        hashmap_free(m->dynamic_users);

        hashmap_free(m->units);
        hashmap_free(m->jobs);
        hashmap_free(m->watch_pids1);
        hashmap_free(m->watch_pids2);
        hashmap_free(m->watch_bus);

        set_free(m->startup_units);
        set_free(m->failed_units);

        sd_event_source_unref(m->signal_event_source);
        sd_event_source_unref(m->notify_event_source);
        sd_event_source_unref(m->cgroups_agent_event_source);
        sd_event_source_unref(m->time_change_event_source);
        sd_event_source_unref(m->jobs_in_progress_event_source);
        sd_event_source_unref(m->run_queue_event_source);
        sd_event_source_unref(m->user_lookup_event_source);

        safe_close(m->signal_fd);
        safe_close(m->notify_fd);
        safe_close(m->cgroups_agent_fd);
        safe_close(m->time_change_fd);
        safe_close_pair(m->user_lookup_fds);

        manager_close_ask_password(m);

        manager_close_idle_pipe(m);

        udev_unref(m->udev);
        sd_event_unref(m->event);

        free(m->notify_socket);

        lookup_paths_free(&m->lookup_paths);
        strv_free(m->environment);

        hashmap_free(m->cgroup_unit);
        set_free_free(m->unit_path_cache);

        free(m->switch_root);
        free(m->switch_root_init);

        for (i = 0; i < _RLIMIT_MAX; i++)
                m->rlimit[i] = mfree(m->rlimit[i]);

        assert(hashmap_isempty(m->units_requiring_mounts_for));
        hashmap_free(m->units_requiring_mounts_for);

        hashmap_free(m->uid_refs);
        hashmap_free(m->gid_refs);

        free(m);
        return NULL;
}