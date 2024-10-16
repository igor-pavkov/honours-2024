int unit_ref_uid_gid(Unit *u, uid_t uid, gid_t gid) {
        ExecContext *c;
        int r;

        assert(u);

        c = unit_get_exec_context(u);

        r = unit_ref_uid_gid_internal(u, uid, gid, c ? c->remove_ipc : false);
        if (r < 0)
                return log_unit_warning_errno(u, r, "Couldn't add UID/GID reference to unit, proceeding without: %m");

        return r;
}