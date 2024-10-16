void unit_dump(Unit *u, FILE *f, const char *prefix) {
        char *t, **j;
        UnitDependency d;
        Iterator i;
        const char *prefix2;
        char
                timestamp0[FORMAT_TIMESTAMP_MAX],
                timestamp1[FORMAT_TIMESTAMP_MAX],
                timestamp2[FORMAT_TIMESTAMP_MAX],
                timestamp3[FORMAT_TIMESTAMP_MAX],
                timestamp4[FORMAT_TIMESTAMP_MAX],
                timespan[FORMAT_TIMESPAN_MAX];
        Unit *following;
        _cleanup_set_free_ Set *following_set = NULL;
        const char *n;
        CGroupMask m;
        int r;

        assert(u);
        assert(u->type >= 0);

        prefix = strempty(prefix);
        prefix2 = strjoina(prefix, "\t");

        fprintf(f,
                "%s-> Unit %s:\n"
                "%s\tDescription: %s\n"
                "%s\tInstance: %s\n"
                "%s\tUnit Load State: %s\n"
                "%s\tUnit Active State: %s\n"
                "%s\tState Change Timestamp: %s\n"
                "%s\tInactive Exit Timestamp: %s\n"
                "%s\tActive Enter Timestamp: %s\n"
                "%s\tActive Exit Timestamp: %s\n"
                "%s\tInactive Enter Timestamp: %s\n"
                "%s\tMay GC: %s\n"
                "%s\tNeed Daemon Reload: %s\n"
                "%s\tTransient: %s\n"
                "%s\tPerpetual: %s\n"
                "%s\tGarbage Collection Mode: %s\n"
                "%s\tSlice: %s\n"
                "%s\tCGroup: %s\n"
                "%s\tCGroup realized: %s\n",
                prefix, u->id,
                prefix, unit_description(u),
                prefix, strna(u->instance),
                prefix, unit_load_state_to_string(u->load_state),
                prefix, unit_active_state_to_string(unit_active_state(u)),
                prefix, strna(format_timestamp(timestamp0, sizeof(timestamp0), u->state_change_timestamp.realtime)),
                prefix, strna(format_timestamp(timestamp1, sizeof(timestamp1), u->inactive_exit_timestamp.realtime)),
                prefix, strna(format_timestamp(timestamp2, sizeof(timestamp2), u->active_enter_timestamp.realtime)),
                prefix, strna(format_timestamp(timestamp3, sizeof(timestamp3), u->active_exit_timestamp.realtime)),
                prefix, strna(format_timestamp(timestamp4, sizeof(timestamp4), u->inactive_enter_timestamp.realtime)),
                prefix, yes_no(unit_may_gc(u)),
                prefix, yes_no(unit_need_daemon_reload(u)),
                prefix, yes_no(u->transient),
                prefix, yes_no(u->perpetual),
                prefix, collect_mode_to_string(u->collect_mode),
                prefix, strna(unit_slice_name(u)),
                prefix, strna(u->cgroup_path),
                prefix, yes_no(u->cgroup_realized));

        if (u->cgroup_realized_mask != 0) {
                _cleanup_free_ char *s = NULL;
                (void) cg_mask_to_string(u->cgroup_realized_mask, &s);
                fprintf(f, "%s\tCGroup realized mask: %s\n", prefix, strnull(s));
        }

        if (u->cgroup_enabled_mask != 0) {
                _cleanup_free_ char *s = NULL;
                (void) cg_mask_to_string(u->cgroup_enabled_mask, &s);
                fprintf(f, "%s\tCGroup enabled mask: %s\n", prefix, strnull(s));
        }

        m = unit_get_own_mask(u);
        if (m != 0) {
                _cleanup_free_ char *s = NULL;
                (void) cg_mask_to_string(m, &s);
                fprintf(f, "%s\tCGroup own mask: %s\n", prefix, strnull(s));
        }

        m = unit_get_members_mask(u);
        if (m != 0) {
                _cleanup_free_ char *s = NULL;
                (void) cg_mask_to_string(m, &s);
                fprintf(f, "%s\tCGroup members mask: %s\n", prefix, strnull(s));
        }

        m = unit_get_delegate_mask(u);
        if (m != 0) {
                _cleanup_free_ char *s = NULL;
                (void) cg_mask_to_string(m, &s);
                fprintf(f, "%s\tCGroup delegate mask: %s\n", prefix, strnull(s));
        }

        SET_FOREACH(t, u->names, i)
                fprintf(f, "%s\tName: %s\n", prefix, t);

        if (!sd_id128_is_null(u->invocation_id))
                fprintf(f, "%s\tInvocation ID: " SD_ID128_FORMAT_STR "\n",
                        prefix, SD_ID128_FORMAT_VAL(u->invocation_id));

        STRV_FOREACH(j, u->documentation)
                fprintf(f, "%s\tDocumentation: %s\n", prefix, *j);

        following = unit_following(u);
        if (following)
                fprintf(f, "%s\tFollowing: %s\n", prefix, following->id);

        r = unit_following_set(u, &following_set);
        if (r >= 0) {
                Unit *other;

                SET_FOREACH(other, following_set, i)
                        fprintf(f, "%s\tFollowing Set Member: %s\n", prefix, other->id);
        }

        if (u->fragment_path)
                fprintf(f, "%s\tFragment Path: %s\n", prefix, u->fragment_path);

        if (u->source_path)
                fprintf(f, "%s\tSource Path: %s\n", prefix, u->source_path);

        STRV_FOREACH(j, u->dropin_paths)
                fprintf(f, "%s\tDropIn Path: %s\n", prefix, *j);

        if (u->failure_action != EMERGENCY_ACTION_NONE)
                fprintf(f, "%s\tFailure Action: %s\n", prefix, emergency_action_to_string(u->failure_action));
        if (u->failure_action_exit_status >= 0)
                fprintf(f, "%s\tFailure Action Exit Status: %i\n", prefix, u->failure_action_exit_status);
        if (u->success_action != EMERGENCY_ACTION_NONE)
                fprintf(f, "%s\tSuccess Action: %s\n", prefix, emergency_action_to_string(u->success_action));
        if (u->success_action_exit_status >= 0)
                fprintf(f, "%s\tSuccess Action Exit Status: %i\n", prefix, u->success_action_exit_status);

        if (u->job_timeout != USEC_INFINITY)
                fprintf(f, "%s\tJob Timeout: %s\n", prefix, format_timespan(timespan, sizeof(timespan), u->job_timeout, 0));

        if (u->job_timeout_action != EMERGENCY_ACTION_NONE)
                fprintf(f, "%s\tJob Timeout Action: %s\n", prefix, emergency_action_to_string(u->job_timeout_action));

        if (u->job_timeout_reboot_arg)
                fprintf(f, "%s\tJob Timeout Reboot Argument: %s\n", prefix, u->job_timeout_reboot_arg);

        condition_dump_list(u->conditions, f, prefix, condition_type_to_string);
        condition_dump_list(u->asserts, f, prefix, assert_type_to_string);

        if (dual_timestamp_is_set(&u->condition_timestamp))
                fprintf(f,
                        "%s\tCondition Timestamp: %s\n"
                        "%s\tCondition Result: %s\n",
                        prefix, strna(format_timestamp(timestamp1, sizeof(timestamp1), u->condition_timestamp.realtime)),
                        prefix, yes_no(u->condition_result));

        if (dual_timestamp_is_set(&u->assert_timestamp))
                fprintf(f,
                        "%s\tAssert Timestamp: %s\n"
                        "%s\tAssert Result: %s\n",
                        prefix, strna(format_timestamp(timestamp1, sizeof(timestamp1), u->assert_timestamp.realtime)),
                        prefix, yes_no(u->assert_result));

        for (d = 0; d < _UNIT_DEPENDENCY_MAX; d++) {
                UnitDependencyInfo di;
                Unit *other;

                HASHMAP_FOREACH_KEY(di.data, other, u->dependencies[d], i) {
                        bool space = false;

                        fprintf(f, "%s\t%s: %s (", prefix, unit_dependency_to_string(d), other->id);

                        print_unit_dependency_mask(f, "origin", di.origin_mask, &space);
                        print_unit_dependency_mask(f, "destination", di.destination_mask, &space);

                        fputs(")\n", f);
                }
        }

        if (!hashmap_isempty(u->requires_mounts_for)) {
                UnitDependencyInfo di;
                const char *path;

                HASHMAP_FOREACH_KEY(di.data, path, u->requires_mounts_for, i) {
                        bool space = false;

                        fprintf(f, "%s\tRequiresMountsFor: %s (", prefix, path);

                        print_unit_dependency_mask(f, "origin", di.origin_mask, &space);
                        print_unit_dependency_mask(f, "destination", di.destination_mask, &space);

                        fputs(")\n", f);
                }
        }

        if (u->load_state == UNIT_LOADED) {

                fprintf(f,
                        "%s\tStopWhenUnneeded: %s\n"
                        "%s\tRefuseManualStart: %s\n"
                        "%s\tRefuseManualStop: %s\n"
                        "%s\tDefaultDependencies: %s\n"
                        "%s\tOnFailureJobMode: %s\n"
                        "%s\tIgnoreOnIsolate: %s\n",
                        prefix, yes_no(u->stop_when_unneeded),
                        prefix, yes_no(u->refuse_manual_start),
                        prefix, yes_no(u->refuse_manual_stop),
                        prefix, yes_no(u->default_dependencies),
                        prefix, job_mode_to_string(u->on_failure_job_mode),
                        prefix, yes_no(u->ignore_on_isolate));

                if (UNIT_VTABLE(u)->dump)
                        UNIT_VTABLE(u)->dump(u, f, prefix2);

        } else if (u->load_state == UNIT_MERGED)
                fprintf(f,
                        "%s\tMerged into: %s\n",
                        prefix, u->merged_into->id);
        else if (u->load_state == UNIT_ERROR)
                fprintf(f, "%s\tLoad Error Code: %s\n", prefix, strerror(-u->load_error));

        for (n = sd_bus_track_first(u->bus_track); n; n = sd_bus_track_next(u->bus_track))
                fprintf(f, "%s\tBus Ref: %s\n", prefix, n);

        if (u->job)
                job_dump(u->job, f, prefix2);

        if (u->nop_job)
                job_dump(u->nop_job, f, prefix2);
}