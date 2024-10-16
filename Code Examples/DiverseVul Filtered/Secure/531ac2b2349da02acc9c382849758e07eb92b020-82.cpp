int manager_new(UnitFileScope scope, bool test_run, Manager **_m) {
        Manager *m;
        int r;

        assert(_m);
        assert(IN_SET(scope, UNIT_FILE_SYSTEM, UNIT_FILE_USER));

        m = new0(Manager, 1);
        if (!m)
                return -ENOMEM;

        m->unit_file_scope = scope;
        m->exit_code = _MANAGER_EXIT_CODE_INVALID;
        m->default_timer_accuracy_usec = USEC_PER_MINUTE;
        m->default_tasks_accounting = true;
        m->default_tasks_max = UINT64_MAX;

#ifdef ENABLE_EFI
        if (MANAGER_IS_SYSTEM(m) && detect_container() <= 0)
                boot_timestamps(&m->userspace_timestamp, &m->firmware_timestamp, &m->loader_timestamp);
#endif

        /* Prepare log fields we can use for structured logging */
        if (MANAGER_IS_SYSTEM(m)) {
                m->unit_log_field = "UNIT=";
                m->unit_log_format_string = "UNIT=%s";
        } else {
                m->unit_log_field = "USER_UNIT=";
                m->unit_log_format_string = "USER_UNIT=%s";
        }

        m->idle_pipe[0] = m->idle_pipe[1] = m->idle_pipe[2] = m->idle_pipe[3] = -1;

        m->pin_cgroupfs_fd = m->notify_fd = m->cgroups_agent_fd = m->signal_fd = m->time_change_fd =
                m->dev_autofs_fd = m->private_listen_fd = m->cgroup_inotify_fd =
                m->ask_password_inotify_fd = -1;

        m->user_lookup_fds[0] = m->user_lookup_fds[1] = -1;

        m->current_job_id = 1; /* start as id #1, so that we can leave #0 around as "null-like" value */

        m->have_ask_password = -EINVAL; /* we don't know */
        m->first_boot = -1;

        m->test_run = test_run;

        /* Reboot immediately if the user hits C-A-D more often than 7x per 2s */
        RATELIMIT_INIT(m->ctrl_alt_del_ratelimit, 2 * USEC_PER_SEC, 7);

        r = manager_default_environment(m);
        if (r < 0)
                goto fail;

        r = hashmap_ensure_allocated(&m->units, &string_hash_ops);
        if (r < 0)
                goto fail;

        r = hashmap_ensure_allocated(&m->jobs, NULL);
        if (r < 0)
                goto fail;

        r = hashmap_ensure_allocated(&m->cgroup_unit, &string_hash_ops);
        if (r < 0)
                goto fail;

        r = hashmap_ensure_allocated(&m->watch_bus, &string_hash_ops);
        if (r < 0)
                goto fail;

        r = sd_event_default(&m->event);
        if (r < 0)
                goto fail;

        r = sd_event_add_defer(m->event, &m->run_queue_event_source, manager_dispatch_run_queue, m);
        if (r < 0)
                goto fail;

        r = sd_event_source_set_priority(m->run_queue_event_source, SD_EVENT_PRIORITY_IDLE);
        if (r < 0)
                goto fail;

        r = sd_event_source_set_enabled(m->run_queue_event_source, SD_EVENT_OFF);
        if (r < 0)
                goto fail;

        (void) sd_event_source_set_description(m->run_queue_event_source, "manager-run-queue");

        r = manager_setup_signals(m);
        if (r < 0)
                goto fail;

        r = manager_setup_cgroup(m);
        if (r < 0)
                goto fail;

        r = manager_setup_time_change(m);
        if (r < 0)
                goto fail;

        m->udev = udev_new();
        if (!m->udev) {
                r = -ENOMEM;
                goto fail;
        }

        /* Note that we do not set up the notify fd here. We do that after deserialization,
         * since they might have gotten serialized across the reexec. */

        m->taint_usr = dir_is_empty("/usr") > 0;

        *_m = m;
        return 0;

fail:
        manager_free(m);
        return r;
}