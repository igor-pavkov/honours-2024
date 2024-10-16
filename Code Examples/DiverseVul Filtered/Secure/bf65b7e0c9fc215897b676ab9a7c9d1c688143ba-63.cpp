void unit_free(Unit *u) {
        UnitDependency d;
        Iterator i;
        char *t;

        if (!u)
                return;

        if (UNIT_ISSET(u->slice)) {
                /* A unit is being dropped from the tree, make sure our parent slice recalculates the member mask */
                unit_invalidate_cgroup_members_masks(UNIT_DEREF(u->slice));

                /* And make sure the parent is realized again, updating cgroup memberships */
                unit_add_to_cgroup_realize_queue(UNIT_DEREF(u->slice));
        }

        u->transient_file = safe_fclose(u->transient_file);

        if (!MANAGER_IS_RELOADING(u->manager))
                unit_remove_transient(u);

        bus_unit_send_removed_signal(u);

        unit_done(u);

        unit_dequeue_rewatch_pids(u);

        sd_bus_slot_unref(u->match_bus_slot);
        sd_bus_track_unref(u->bus_track);
        u->deserialized_refs = strv_free(u->deserialized_refs);

        unit_free_requires_mounts_for(u);

        SET_FOREACH(t, u->names, i)
                hashmap_remove_value(u->manager->units, t, u);

        if (!sd_id128_is_null(u->invocation_id))
                hashmap_remove_value(u->manager->units_by_invocation_id, &u->invocation_id, u);

        if (u->job) {
                Job *j = u->job;
                job_uninstall(j);
                job_free(j);
        }

        if (u->nop_job) {
                Job *j = u->nop_job;
                job_uninstall(j);
                job_free(j);
        }

        for (d = 0; d < _UNIT_DEPENDENCY_MAX; d++)
                bidi_set_free(u, u->dependencies[d]);

        if (u->on_console)
                manager_unref_console(u->manager);

        unit_release_cgroup(u);

        if (!MANAGER_IS_RELOADING(u->manager))
                unit_unlink_state_files(u);

        unit_unref_uid_gid(u, false);

        (void) manager_update_failed_units(u->manager, u, false);
        set_remove(u->manager->startup_units, u);

        unit_unwatch_all_pids(u);

        unit_ref_unset(&u->slice);
        while (u->refs_by_target)
                unit_ref_unset(u->refs_by_target);

        if (u->type != _UNIT_TYPE_INVALID)
                LIST_REMOVE(units_by_type, u->manager->units_by_type[u->type], u);

        if (u->in_load_queue)
                LIST_REMOVE(load_queue, u->manager->load_queue, u);

        if (u->in_dbus_queue)
                LIST_REMOVE(dbus_queue, u->manager->dbus_unit_queue, u);

        if (u->in_gc_queue)
                LIST_REMOVE(gc_queue, u->manager->gc_unit_queue, u);

        if (u->in_cgroup_realize_queue)
                LIST_REMOVE(cgroup_realize_queue, u->manager->cgroup_realize_queue, u);

        if (u->in_cgroup_empty_queue)
                LIST_REMOVE(cgroup_empty_queue, u->manager->cgroup_empty_queue, u);

        if (u->in_cleanup_queue)
                LIST_REMOVE(cleanup_queue, u->manager->cleanup_queue, u);

        if (u->in_target_deps_queue)
                LIST_REMOVE(target_deps_queue, u->manager->target_deps_queue, u);

        if (u->in_stop_when_unneeded_queue)
                LIST_REMOVE(stop_when_unneeded_queue, u->manager->stop_when_unneeded_queue, u);

        safe_close(u->ip_accounting_ingress_map_fd);
        safe_close(u->ip_accounting_egress_map_fd);

        safe_close(u->ipv4_allow_map_fd);
        safe_close(u->ipv6_allow_map_fd);
        safe_close(u->ipv4_deny_map_fd);
        safe_close(u->ipv6_deny_map_fd);

        bpf_program_unref(u->ip_bpf_ingress);
        bpf_program_unref(u->ip_bpf_ingress_installed);
        bpf_program_unref(u->ip_bpf_egress);
        bpf_program_unref(u->ip_bpf_egress_installed);

        bpf_program_unref(u->bpf_device_control_installed);

        condition_free_list(u->conditions);
        condition_free_list(u->asserts);

        free(u->description);
        strv_free(u->documentation);
        free(u->fragment_path);
        free(u->source_path);
        strv_free(u->dropin_paths);
        free(u->instance);

        free(u->job_timeout_reboot_arg);

        set_free_free(u->names);

        free(u->reboot_arg);

        free(u);
}