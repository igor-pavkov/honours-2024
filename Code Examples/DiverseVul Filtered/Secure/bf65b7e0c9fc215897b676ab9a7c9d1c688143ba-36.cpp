static int unit_export_invocation_id(Unit *u) {
        const char *p;
        int r;

        assert(u);

        if (u->exported_invocation_id)
                return 0;

        if (sd_id128_is_null(u->invocation_id))
                return 0;

        p = strjoina("/run/systemd/units/invocation:", u->id);
        r = symlink_atomic(u->invocation_id_string, p);
        if (r < 0)
                return log_unit_debug_errno(u, r, "Failed to create invocation ID symlink %s: %m", p);

        u->exported_invocation_id = true;
        return 0;
}