static int unit_export_log_rate_limit_burst(Unit *u, const ExecContext *c) {
        _cleanup_free_ char *buf = NULL;
        const char *p;
        int r;

        assert(u);
        assert(c);

        if (u->exported_log_rate_limit_burst)
                return 0;

        if (c->log_rate_limit_burst == 0)
                return 0;

        p = strjoina("/run/systemd/units/log-rate-limit-burst:", u->id);

        if (asprintf(&buf, "%u", c->log_rate_limit_burst) < 0)
                return log_oom();

        r = symlink_atomic(buf, p);
        if (r < 0)
                return log_unit_debug_errno(u, r, "Failed to create log rate limit burst symlink %s: %m", p);

        u->exported_log_rate_limit_burst = true;
        return 0;
}