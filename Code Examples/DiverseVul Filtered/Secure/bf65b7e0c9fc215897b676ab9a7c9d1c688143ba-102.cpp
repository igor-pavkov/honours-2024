int unit_serialize(Unit *u, FILE *f, FDSet *fds, bool serialize_jobs) {
        CGroupIPAccountingMetric m;
        int r;

        assert(u);
        assert(f);
        assert(fds);

        if (unit_can_serialize(u)) {
                r = UNIT_VTABLE(u)->serialize(u, f, fds);
                if (r < 0)
                        return r;
        }

        (void) serialize_dual_timestamp(f, "state-change-timestamp", &u->state_change_timestamp);

        (void) serialize_dual_timestamp(f, "inactive-exit-timestamp", &u->inactive_exit_timestamp);
        (void) serialize_dual_timestamp(f, "active-enter-timestamp", &u->active_enter_timestamp);
        (void) serialize_dual_timestamp(f, "active-exit-timestamp", &u->active_exit_timestamp);
        (void) serialize_dual_timestamp(f, "inactive-enter-timestamp", &u->inactive_enter_timestamp);

        (void) serialize_dual_timestamp(f, "condition-timestamp", &u->condition_timestamp);
        (void) serialize_dual_timestamp(f, "assert-timestamp", &u->assert_timestamp);

        if (dual_timestamp_is_set(&u->condition_timestamp))
                (void) serialize_bool(f, "condition-result", u->condition_result);

        if (dual_timestamp_is_set(&u->assert_timestamp))
                (void) serialize_bool(f, "assert-result", u->assert_result);

        (void) serialize_bool(f, "transient", u->transient);
        (void) serialize_bool(f, "in-audit", u->in_audit);

        (void) serialize_bool(f, "exported-invocation-id", u->exported_invocation_id);
        (void) serialize_bool(f, "exported-log-level-max", u->exported_log_level_max);
        (void) serialize_bool(f, "exported-log-extra-fields", u->exported_log_extra_fields);
        (void) serialize_bool(f, "exported-log-rate-limit-interval", u->exported_log_rate_limit_interval);
        (void) serialize_bool(f, "exported-log-rate-limit-burst", u->exported_log_rate_limit_burst);

        (void) serialize_item_format(f, "cpu-usage-base", "%" PRIu64, u->cpu_usage_base);
        if (u->cpu_usage_last != NSEC_INFINITY)
                (void) serialize_item_format(f, "cpu-usage-last", "%" PRIu64, u->cpu_usage_last);

        if (u->cgroup_path)
                (void) serialize_item(f, "cgroup", u->cgroup_path);

        (void) serialize_bool(f, "cgroup-realized", u->cgroup_realized);
        (void) serialize_cgroup_mask(f, "cgroup-realized-mask", u->cgroup_realized_mask);
        (void) serialize_cgroup_mask(f, "cgroup-enabled-mask", u->cgroup_enabled_mask);
        (void) serialize_cgroup_mask(f, "cgroup-invalidated-mask", u->cgroup_invalidated_mask);

        if (uid_is_valid(u->ref_uid))
                (void) serialize_item_format(f, "ref-uid", UID_FMT, u->ref_uid);
        if (gid_is_valid(u->ref_gid))
                (void) serialize_item_format(f, "ref-gid", GID_FMT, u->ref_gid);

        if (!sd_id128_is_null(u->invocation_id))
                (void) serialize_item_format(f, "invocation-id", SD_ID128_FORMAT_STR, SD_ID128_FORMAT_VAL(u->invocation_id));

        bus_track_serialize(u->bus_track, f, "ref");

        for (m = 0; m < _CGROUP_IP_ACCOUNTING_METRIC_MAX; m++) {
                uint64_t v;

                r = unit_get_ip_accounting(u, m, &v);
                if (r >= 0)
                        (void) serialize_item_format(f, ip_accounting_metric_field[m], "%" PRIu64, v);
        }

        if (serialize_jobs) {
                if (u->job) {
                        fputs("job\n", f);
                        job_serialize(u->job, f);
                }

                if (u->nop_job) {
                        fputs("job\n", f);
                        job_serialize(u->nop_job, f);
                }
        }

        /* End marker */
        fputc('\n', f);
        return 0;
}