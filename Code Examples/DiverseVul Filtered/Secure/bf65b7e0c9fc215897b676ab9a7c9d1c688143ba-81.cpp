static void unit_unref_uid_internal(
                Unit *u,
                uid_t *ref_uid,
                bool destroy_now,
                void (*_manager_unref_uid)(Manager *m, uid_t uid, bool destroy_now)) {

        assert(u);
        assert(ref_uid);
        assert(_manager_unref_uid);

        /* Generic implementation of both unit_unref_uid() and unit_unref_gid(), under the assumption that uid_t and
         * gid_t are actually the same time, with the same validity rules.
         *
         * Drops a reference to UID/GID from a unit. */

        assert_cc(sizeof(uid_t) == sizeof(gid_t));
        assert_cc(UID_INVALID == (uid_t) GID_INVALID);

        if (!uid_is_valid(*ref_uid))
                return;

        _manager_unref_uid(u->manager, *ref_uid, destroy_now);
        *ref_uid = UID_INVALID;
}