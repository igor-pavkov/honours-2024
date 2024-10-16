void unit_warn_if_dir_nonempty(Unit *u, const char* where) {
        int r;

        assert(u);
        assert(where);

        r = dir_is_empty(where);
        if (r > 0 || r == -ENOTDIR)
                return;
        if (r < 0) {
                log_unit_warning_errno(u, r, "Failed to check directory %s: %m", where);
                return;
        }

        log_struct(LOG_NOTICE,
                   "MESSAGE_ID=" SD_MESSAGE_OVERMOUNTING_STR,
                   LOG_UNIT_ID(u),
                   LOG_UNIT_INVOCATION_ID(u),
                   LOG_UNIT_MESSAGE(u, "Directory %s to mount over is not empty, mounting anyway.", where),
                   "WHERE=%s", where);
}