static int unit_ref_uid_gid_internal(Unit *u, uid_t uid, gid_t gid, bool clean_ipc) {
        int r = 0, q = 0;

        assert(u);

        /* Reference both a UID and a GID in one go. Either references both, or neither. */

        if (uid_is_valid(uid)) {
                r = unit_ref_uid(u, uid, clean_ipc);
                if (r < 0)
                        return r;
        }

        if (gid_is_valid(gid)) {
                q = unit_ref_gid(u, gid, clean_ipc);
                if (q < 0) {
                        if (r > 0)
                                unit_unref_uid(u, false);

                        return q;
                }
        }

        return r > 0 || q > 0;
}