int unit_deserialize(Unit *u, FILE *f, FDSet *fds) {
        int r;

        assert(u);
        assert(f);
        assert(fds);

        for (;;) {
                _cleanup_free_ char *line = NULL;
                CGroupIPAccountingMetric m;
                char *l, *v;
                size_t k;

                r = read_line(f, LONG_LINE_MAX, &line);
                if (r < 0)
                        return log_error_errno(r, "Failed to read serialization line: %m");
                if (r == 0) /* eof */
                        break;

                l = strstrip(line);
                if (isempty(l)) /* End marker */
                        break;

                k = strcspn(l, "=");

                if (l[k] == '=') {
                        l[k] = 0;
                        v = l+k+1;
                } else
                        v = l+k;

                if (streq(l, "job")) {
                        if (v[0] == '\0') {
                                /* New-style serialized job */
                                r = unit_deserialize_job(u, f);
                                if (r < 0)
                                        return r;
                        } else  /* Legacy for pre-44 */
                                log_unit_warning(u, "Update from too old systemd versions are unsupported, cannot deserialize job: %s", v);
                        continue;
                } else if (streq(l, "state-change-timestamp")) {
                        (void) deserialize_dual_timestamp(v, &u->state_change_timestamp);
                        continue;
                } else if (streq(l, "inactive-exit-timestamp")) {
                        (void) deserialize_dual_timestamp(v, &u->inactive_exit_timestamp);
                        continue;
                } else if (streq(l, "active-enter-timestamp")) {
                        (void) deserialize_dual_timestamp(v, &u->active_enter_timestamp);
                        continue;
                } else if (streq(l, "active-exit-timestamp")) {
                        (void) deserialize_dual_timestamp(v, &u->active_exit_timestamp);
                        continue;
                } else if (streq(l, "inactive-enter-timestamp")) {
                        (void) deserialize_dual_timestamp(v, &u->inactive_enter_timestamp);
                        continue;
                } else if (streq(l, "condition-timestamp")) {
                        (void) deserialize_dual_timestamp(v, &u->condition_timestamp);
                        continue;
                } else if (streq(l, "assert-timestamp")) {
                        (void) deserialize_dual_timestamp(v, &u->assert_timestamp);
                        continue;
                } else if (streq(l, "condition-result")) {

                        r = parse_boolean(v);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse condition result value %s, ignoring.", v);
                        else
                                u->condition_result = r;

                        continue;

                } else if (streq(l, "assert-result")) {

                        r = parse_boolean(v);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse assert result value %s, ignoring.", v);
                        else
                                u->assert_result = r;

                        continue;

                } else if (streq(l, "transient")) {

                        r = parse_boolean(v);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse transient bool %s, ignoring.", v);
                        else
                                u->transient = r;

                        continue;

                } else if (streq(l, "in-audit")) {

                        r = parse_boolean(v);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse in-audit bool %s, ignoring.", v);
                        else
                                u->in_audit = r;

                        continue;

                } else if (streq(l, "exported-invocation-id")) {

                        r = parse_boolean(v);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse exported invocation ID bool %s, ignoring.", v);
                        else
                                u->exported_invocation_id = r;

                        continue;

                } else if (streq(l, "exported-log-level-max")) {

                        r = parse_boolean(v);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse exported log level max bool %s, ignoring.", v);
                        else
                                u->exported_log_level_max = r;

                        continue;

                } else if (streq(l, "exported-log-extra-fields")) {

                        r = parse_boolean(v);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse exported log extra fields bool %s, ignoring.", v);
                        else
                                u->exported_log_extra_fields = r;

                        continue;

                } else if (streq(l, "exported-log-rate-limit-interval")) {

                        r = parse_boolean(v);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse exported log rate limit interval %s, ignoring.", v);
                        else
                                u->exported_log_rate_limit_interval = r;

                        continue;

                } else if (streq(l, "exported-log-rate-limit-burst")) {

                        r = parse_boolean(v);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse exported log rate limit burst %s, ignoring.", v);
                        else
                                u->exported_log_rate_limit_burst = r;

                        continue;

                } else if (STR_IN_SET(l, "cpu-usage-base", "cpuacct-usage-base")) {

                        r = safe_atou64(v, &u->cpu_usage_base);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse CPU usage base %s, ignoring.", v);

                        continue;

                } else if (streq(l, "cpu-usage-last")) {

                        r = safe_atou64(v, &u->cpu_usage_last);
                        if (r < 0)
                                log_unit_debug(u, "Failed to read CPU usage last %s, ignoring.", v);

                        continue;

                } else if (streq(l, "cgroup")) {

                        r = unit_set_cgroup_path(u, v);
                        if (r < 0)
                                log_unit_debug_errno(u, r, "Failed to set cgroup path %s, ignoring: %m", v);

                        (void) unit_watch_cgroup(u);

                        continue;
                } else if (streq(l, "cgroup-realized")) {
                        int b;

                        b = parse_boolean(v);
                        if (b < 0)
                                log_unit_debug(u, "Failed to parse cgroup-realized bool %s, ignoring.", v);
                        else
                                u->cgroup_realized = b;

                        continue;

                } else if (streq(l, "cgroup-realized-mask")) {

                        r = cg_mask_from_string(v, &u->cgroup_realized_mask);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse cgroup-realized-mask %s, ignoring.", v);
                        continue;

                } else if (streq(l, "cgroup-enabled-mask")) {

                        r = cg_mask_from_string(v, &u->cgroup_enabled_mask);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse cgroup-enabled-mask %s, ignoring.", v);
                        continue;

                } else if (streq(l, "cgroup-invalidated-mask")) {

                        r = cg_mask_from_string(v, &u->cgroup_invalidated_mask);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse cgroup-invalidated-mask %s, ignoring.", v);
                        continue;

                } else if (streq(l, "ref-uid")) {
                        uid_t uid;

                        r = parse_uid(v, &uid);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse referenced UID %s, ignoring.", v);
                        else
                                unit_ref_uid_gid(u, uid, GID_INVALID);

                        continue;

                } else if (streq(l, "ref-gid")) {
                        gid_t gid;

                        r = parse_gid(v, &gid);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse referenced GID %s, ignoring.", v);
                        else
                                unit_ref_uid_gid(u, UID_INVALID, gid);

                        continue;

                } else if (streq(l, "ref")) {

                        r = strv_extend(&u->deserialized_refs, v);
                        if (r < 0)
                                return log_oom();

                        continue;
                } else if (streq(l, "invocation-id")) {
                        sd_id128_t id;

                        r = sd_id128_from_string(v, &id);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse invocation id %s, ignoring.", v);
                        else {
                                r = unit_set_invocation_id(u, id);
                                if (r < 0)
                                        log_unit_warning_errno(u, r, "Failed to set invocation ID for unit: %m");
                        }

                        continue;
                }

                /* Check if this is an IP accounting metric serialization field */
                for (m = 0; m < _CGROUP_IP_ACCOUNTING_METRIC_MAX; m++)
                        if (streq(l, ip_accounting_metric_field[m]))
                                break;
                if (m < _CGROUP_IP_ACCOUNTING_METRIC_MAX) {
                        uint64_t c;

                        r = safe_atou64(v, &c);
                        if (r < 0)
                                log_unit_debug(u, "Failed to parse IP accounting value %s, ignoring.", v);
                        else
                                u->ip_accounting_extra[m] = c;
                        continue;
                }

                if (unit_can_serialize(u)) {
                        r = exec_runtime_deserialize_compat(u, l, v, fds);
                        if (r < 0) {
                                log_unit_warning(u, "Failed to deserialize runtime parameter '%s', ignoring.", l);
                                continue;
                        }

                        /* Returns positive if key was handled by the call */
                        if (r > 0)
                                continue;

                        r = UNIT_VTABLE(u)->deserialize_item(u, l, v, fds);
                        if (r < 0)
                                log_unit_warning(u, "Failed to deserialize unit parameter '%s', ignoring.", l);
                }
        }

        /* Versions before 228 did not carry a state change timestamp. In this case, take the current time. This is
         * useful, so that timeouts based on this timestamp don't trigger too early, and is in-line with the logic from
         * before 228 where the base for timeouts was not persistent across reboots. */

        if (!dual_timestamp_is_set(&u->state_change_timestamp))
                dual_timestamp_get(&u->state_change_timestamp);

        /* Let's make sure that everything that is deserialized also gets any potential new cgroup settings applied
         * after we are done. For that we invalidate anything already realized, so that we can realize it again. */
        unit_invalidate_cgroup(u, _CGROUP_MASK_ALL);
        unit_invalidate_cgroup_bpf(u);

        return 0;
}