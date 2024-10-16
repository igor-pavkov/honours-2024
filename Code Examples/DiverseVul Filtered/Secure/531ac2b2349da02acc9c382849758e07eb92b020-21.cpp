static void manager_unref_uid_internal(
                Manager *m,
                Hashmap **uid_refs,
                uid_t uid,
                bool destroy_now,
                int (*_clean_ipc)(uid_t uid)) {

        uint32_t c, n;

        assert(m);
        assert(uid_refs);
        assert(uid_is_valid(uid));
        assert(_clean_ipc);

        /* A generic implementation, covering both manager_unref_uid() and manager_unref_gid(), under the assumption
         * that uid_t and gid_t are actually defined the same way, with the same validity rules.
         *
         * We store a hashmap where the UID/GID is they key and the value is a 32bit reference counter, whose highest
         * bit is used as flag for marking UIDs/GIDs whose IPC objects to remove when the last reference to the UID/GID
         * is dropped. The flag is set to on, once at least one reference from a unit where RemoveIPC= is set is added
         * on a UID/GID. It is reset when the UID's/GID's reference counter drops to 0 again. */

        assert_cc(sizeof(uid_t) == sizeof(gid_t));
        assert_cc(UID_INVALID == (uid_t) GID_INVALID);

        if (uid == 0) /* We don't keep track of root, and will never destroy it */
                return;

        c = PTR_TO_UINT32(hashmap_get(*uid_refs, UID_TO_PTR(uid)));

        n = c & ~DESTROY_IPC_FLAG;
        assert(n > 0);
        n--;

        if (destroy_now && n == 0) {
                hashmap_remove(*uid_refs, UID_TO_PTR(uid));

                if (c & DESTROY_IPC_FLAG) {
                        log_debug("%s " UID_FMT " is no longer referenced, cleaning up its IPC.",
                                  _clean_ipc == clean_ipc_by_uid ? "UID" : "GID",
                                  uid);
                        (void) _clean_ipc(uid);
                }
        } else {
                c = n | (c & DESTROY_IPC_FLAG);
                assert_se(hashmap_update(*uid_refs, UID_TO_PTR(uid), UINT32_TO_PTR(c)) >= 0);
        }
}