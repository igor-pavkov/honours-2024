static int unit_ref_uid_internal(
                Unit *u,
                uid_t *ref_uid,
                uid_t uid,
                bool clean_ipc,
                int (*_manager_ref_uid)(Manager *m, uid_t uid, bool clean_ipc)) {

        int r;

        assert(u);
        assert(ref_uid);
        assert(uid_is_valid(uid));
        assert(_manager_ref_uid);

        /* Generic implementation of both unit_ref_uid() and unit_ref_guid(), under the assumption that uid_t and gid_t
         * are actually the same type, and have the same validity rules.
         *
         * Adds a reference on a specific UID/GID to this unit. Each unit referencing the same UID/GID maintains a
         * reference so that we can destroy the UID/GID's IPC resources as soon as this is requested and the counter
         * drops to zero. */

        assert_cc(sizeof(uid_t) == sizeof(gid_t));
        assert_cc(UID_INVALID == (uid_t) GID_INVALID);

        if (*ref_uid == uid)
                return 0;

        if (uid_is_valid(*ref_uid)) /* Already set? */
                return -EBUSY;

        r = _manager_ref_uid(u->manager, uid, clean_ipc);
        if (r < 0)
                return r;

        *ref_uid = uid;
        return 1;
}