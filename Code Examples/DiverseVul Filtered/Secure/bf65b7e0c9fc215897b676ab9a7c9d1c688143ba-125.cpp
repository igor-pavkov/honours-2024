void unit_notify(Unit *u, UnitActiveState os, UnitActiveState ns, UnitNotifyFlags flags) {
        const char *reason;
        Manager *m;

        assert(u);
        assert(os < _UNIT_ACTIVE_STATE_MAX);
        assert(ns < _UNIT_ACTIVE_STATE_MAX);

        /* Note that this is called for all low-level state changes, even if they might map to the same high-level
         * UnitActiveState! That means that ns == os is an expected behavior here. For example: if a mount point is
         * remounted this function will be called too! */

        m = u->manager;

        /* Let's enqueue the change signal early. In case this unit has a job associated we want that this unit is in
         * the bus queue, so that any job change signal queued will force out the unit change signal first. */
        unit_add_to_dbus_queue(u);

        /* Update timestamps for state changes */
        if (!MANAGER_IS_RELOADING(m)) {
                dual_timestamp_get(&u->state_change_timestamp);

                if (UNIT_IS_INACTIVE_OR_FAILED(os) && !UNIT_IS_INACTIVE_OR_FAILED(ns))
                        u->inactive_exit_timestamp = u->state_change_timestamp;
                else if (!UNIT_IS_INACTIVE_OR_FAILED(os) && UNIT_IS_INACTIVE_OR_FAILED(ns))
                        u->inactive_enter_timestamp = u->state_change_timestamp;

                if (!UNIT_IS_ACTIVE_OR_RELOADING(os) && UNIT_IS_ACTIVE_OR_RELOADING(ns))
                        u->active_enter_timestamp = u->state_change_timestamp;
                else if (UNIT_IS_ACTIVE_OR_RELOADING(os) && !UNIT_IS_ACTIVE_OR_RELOADING(ns))
                        u->active_exit_timestamp = u->state_change_timestamp;
        }

        /* Keep track of failed units */
        (void) manager_update_failed_units(m, u, ns == UNIT_FAILED);

        /* Make sure the cgroup and state files are always removed when we become inactive */
        if (UNIT_IS_INACTIVE_OR_FAILED(ns)) {
                unit_prune_cgroup(u);
                unit_unlink_state_files(u);
        }

        unit_update_on_console(u);

        if (!MANAGER_IS_RELOADING(m)) {
                bool unexpected;

                /* Let's propagate state changes to the job */
                if (u->job)
                        unexpected = unit_process_job(u->job, ns, flags);
                else
                        unexpected = true;

                /* If this state change happened without being requested by a job, then let's retroactively start or
                 * stop dependencies. We skip that step when deserializing, since we don't want to create any
                 * additional jobs just because something is already activated. */

                if (unexpected) {
                        if (UNIT_IS_INACTIVE_OR_FAILED(os) && UNIT_IS_ACTIVE_OR_ACTIVATING(ns))
                                retroactively_start_dependencies(u);
                        else if (UNIT_IS_ACTIVE_OR_ACTIVATING(os) && UNIT_IS_INACTIVE_OR_DEACTIVATING(ns))
                                retroactively_stop_dependencies(u);
                }

                /* stop unneeded units regardless if going down was expected or not */
                if (UNIT_IS_INACTIVE_OR_FAILED(ns))
                        check_unneeded_dependencies(u);

                if (ns != os && ns == UNIT_FAILED) {
                        log_unit_debug(u, "Unit entered failed state.");

                        if (!(flags & UNIT_NOTIFY_WILL_AUTO_RESTART))
                                unit_start_on_failure(u);
                }

                if (UNIT_IS_ACTIVE_OR_RELOADING(ns) && !UNIT_IS_ACTIVE_OR_RELOADING(os)) {
                        /* This unit just finished starting up */

                        unit_emit_audit_start(u);
                        manager_send_unit_plymouth(m, u);
                }

                if (UNIT_IS_INACTIVE_OR_FAILED(ns) && !UNIT_IS_INACTIVE_OR_FAILED(os)) {
                        /* This unit just stopped/failed. */

                        unit_emit_audit_stop(u, ns);
                        unit_log_resources(u);
                }
        }

        manager_recheck_journal(m);
        manager_recheck_dbus(m);

        unit_trigger_notify(u);

        if (!MANAGER_IS_RELOADING(m)) {
                /* Maybe we finished startup and are now ready for being stopped because unneeded? */
                unit_submit_to_stop_when_unneeded_queue(u);

                /* Maybe we finished startup, but something we needed has vanished? Let's die then. (This happens when
                 * something BindsTo= to a Type=oneshot unit, as these units go directly from starting to inactive,
                 * without ever entering started.) */
                unit_check_binds_to(u);

                if (os != UNIT_FAILED && ns == UNIT_FAILED) {
                        reason = strjoina("unit ", u->id, " failed");
                        emergency_action(m, u->failure_action, 0, u->reboot_arg, unit_failure_action_exit_status(u), reason);
                } else if (!UNIT_IS_INACTIVE_OR_FAILED(os) && ns == UNIT_INACTIVE) {
                        reason = strjoina("unit ", u->id, " succeeded");
                        emergency_action(m, u->success_action, 0, u->reboot_arg, unit_success_action_exit_status(u), reason);
                }
        }

        unit_add_to_gc_queue(u);
}