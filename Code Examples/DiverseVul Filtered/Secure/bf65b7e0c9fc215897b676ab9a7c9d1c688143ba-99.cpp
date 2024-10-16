static int unit_export_log_level_max(Unit *u, const ExecContext *c) {
        const char *p;
        char buf[2];
        int r;

        assert(u);
        assert(c);

        if (u->exported_log_level_max)
                return 0;

        if (c->log_level_max < 0)
                return 0;

        assert(c->log_level_max <= 7);

        buf[0] = '0' + c->log_level_max;
        buf[1] = 0;

        p = strjoina("/run/systemd/units/log-level-max:", u->id);
        r = symlink_atomic(buf, p);
        if (r < 0)
                return log_unit_debug_errno(u, r, "Failed to create maximum log level symlink %s: %m", p);

        u->exported_log_level_max = true;
        return 0;
}