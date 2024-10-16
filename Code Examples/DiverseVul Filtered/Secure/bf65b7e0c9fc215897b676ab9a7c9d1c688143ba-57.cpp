int unit_load(Unit *u) {
        int r;

        assert(u);

        if (u->in_load_queue) {
                LIST_REMOVE(load_queue, u->manager->load_queue, u);
                u->in_load_queue = false;
        }

        if (u->type == _UNIT_TYPE_INVALID)
                return -EINVAL;

        if (u->load_state != UNIT_STUB)
                return 0;

        if (u->transient_file) {
                /* Finalize transient file: if this is a transient unit file, as soon as we reach unit_load() the setup
                 * is complete, hence let's synchronize the unit file we just wrote to disk. */

                r = fflush_and_check(u->transient_file);
                if (r < 0)
                        goto fail;

                u->transient_file = safe_fclose(u->transient_file);
                u->fragment_mtime = now(CLOCK_REALTIME);
        }

        if (UNIT_VTABLE(u)->load) {
                r = UNIT_VTABLE(u)->load(u);
                if (r < 0)
                        goto fail;
        }

        if (u->load_state == UNIT_STUB) {
                r = -ENOENT;
                goto fail;
        }

        if (u->load_state == UNIT_LOADED) {
                unit_add_to_target_deps_queue(u);

                r = unit_add_slice_dependencies(u);
                if (r < 0)
                        goto fail;

                r = unit_add_mount_dependencies(u);
                if (r < 0)
                        goto fail;

                r = unit_add_startup_units(u);
                if (r < 0)
                        goto fail;

                if (u->on_failure_job_mode == JOB_ISOLATE && hashmap_size(u->dependencies[UNIT_ON_FAILURE]) > 1) {
                        log_unit_error(u, "More than one OnFailure= dependencies specified but OnFailureJobMode=isolate set. Refusing.");
                        r = -ENOEXEC;
                        goto fail;
                }

                if (u->job_running_timeout != USEC_INFINITY && u->job_running_timeout > u->job_timeout)
                        log_unit_warning(u, "JobRunningTimeoutSec= is greater than JobTimeoutSec=, it has no effect.");

                /* We finished loading, let's ensure our parents recalculate the members mask */
                unit_invalidate_cgroup_members_masks(u);
        }

        assert((u->load_state != UNIT_MERGED) == !u->merged_into);

        unit_add_to_dbus_queue(unit_follow_merge(u));
        unit_add_to_gc_queue(u);

        return 0;

fail:
        /* We convert ENOEXEC errors to the UNIT_BAD_SETTING load state here. Configuration parsing code should hence
         * return ENOEXEC to ensure units are placed in this state after loading */

        u->load_state = u->load_state == UNIT_STUB ? UNIT_NOT_FOUND :
                                     r == -ENOEXEC ? UNIT_BAD_SETTING :
                                                     UNIT_ERROR;
        u->load_error = r;

        unit_add_to_dbus_queue(u);
        unit_add_to_gc_queue(u);

        return log_unit_debug_errno(u, r, "Failed to load configuration: %m");
}