int unit_fail_if_noncanonical(Unit *u, const char* where) {
        _cleanup_free_ char *canonical_where = NULL;
        int r;

        assert(u);
        assert(where);

        r = chase_symlinks(where, NULL, CHASE_NONEXISTENT, &canonical_where);
        if (r < 0) {
                log_unit_debug_errno(u, r, "Failed to check %s for symlinks, ignoring: %m", where);
                return 0;
        }

        /* We will happily ignore a trailing slash (or any redundant slashes) */
        if (path_equal(where, canonical_where))
                return 0;

        /* No need to mention "." or "..", they would already have been rejected by unit_name_from_path() */
        log_struct(LOG_ERR,
                   "MESSAGE_ID=" SD_MESSAGE_OVERMOUNTING_STR,
                   LOG_UNIT_ID(u),
                   LOG_UNIT_INVOCATION_ID(u),
                   LOG_UNIT_MESSAGE(u, "Mount path %s is not canonical (contains a symlink).", where),
                   "WHERE=%s", where);

        return -ELOOP;
}