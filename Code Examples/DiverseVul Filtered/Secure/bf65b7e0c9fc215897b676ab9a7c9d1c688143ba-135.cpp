Unit *unit_new(Manager *m, size_t size) {
        Unit *u;

        assert(m);
        assert(size >= sizeof(Unit));

        u = malloc0(size);
        if (!u)
                return NULL;

        u->names = set_new(&string_hash_ops);
        if (!u->names)
                return mfree(u);

        u->manager = m;
        u->type = _UNIT_TYPE_INVALID;
        u->default_dependencies = true;
        u->unit_file_state = _UNIT_FILE_STATE_INVALID;
        u->unit_file_preset = -1;
        u->on_failure_job_mode = JOB_REPLACE;
        u->cgroup_inotify_wd = -1;
        u->job_timeout = USEC_INFINITY;
        u->job_running_timeout = USEC_INFINITY;
        u->ref_uid = UID_INVALID;
        u->ref_gid = GID_INVALID;
        u->cpu_usage_last = NSEC_INFINITY;
        u->cgroup_invalidated_mask |= CGROUP_MASK_BPF_FIREWALL;
        u->failure_action_exit_status = u->success_action_exit_status = -1;

        u->ip_accounting_ingress_map_fd = -1;
        u->ip_accounting_egress_map_fd = -1;
        u->ipv4_allow_map_fd = -1;
        u->ipv6_allow_map_fd = -1;
        u->ipv4_deny_map_fd = -1;
        u->ipv6_deny_map_fd = -1;

        u->last_section_private = -1;

        RATELIMIT_INIT(u->start_limit, m->default_start_limit_interval, m->default_start_limit_burst);
        RATELIMIT_INIT(u->auto_stop_ratelimit, 10 * USEC_PER_SEC, 16);

        return u;
}