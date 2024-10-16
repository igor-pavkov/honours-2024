static void manager_vacuum_uid_refs_internal(
                Manager *m,
                Hashmap **uid_refs,
                int (*_clean_ipc)(uid_t uid)) {

        Iterator i;
        void *p, *k;

        assert(m);
        assert(uid_refs);
        assert(_clean_ipc);

        HASHMAP_FOREACH_KEY(p, k, *uid_refs, i) {
                uint32_t c, n;
                uid_t uid;

                uid = PTR_TO_UID(k);
                c = PTR_TO_UINT32(p);

                n = c & ~DESTROY_IPC_FLAG;
                if (n > 0)
                        continue;

                if (c & DESTROY_IPC_FLAG) {
                        log_debug("Found unreferenced %s " UID_FMT " after reload/reexec. Cleaning up.",
                                  _clean_ipc == clean_ipc_by_uid ? "UID" : "GID",
                                  uid);
                        (void) _clean_ipc(uid);
                }

                assert_se(hashmap_remove(*uid_refs, k) == p);
        }
}