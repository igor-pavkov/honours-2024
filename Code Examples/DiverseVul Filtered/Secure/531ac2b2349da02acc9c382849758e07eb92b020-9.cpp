static int manager_ref_uid_internal(
                Manager *m,
                Hashmap **uid_refs,
                uid_t uid,
                bool clean_ipc) {

        uint32_t c, n;
        int r;

        assert(m);
        assert(uid_refs);
        assert(uid_is_valid(uid));

        /* A generic implementation, covering both manager_ref_uid() and manager_ref_gid(), under the assumption
         * that uid_t and gid_t are actually defined the same way, with the same validity rules. */

        assert_cc(sizeof(uid_t) == sizeof(gid_t));
        assert_cc(UID_INVALID == (uid_t) GID_INVALID);

        if (uid == 0) /* We don't keep track of root, and will never destroy it */
                return 0;

        r = hashmap_ensure_allocated(uid_refs, &trivial_hash_ops);
        if (r < 0)
                return r;

        c = PTR_TO_UINT32(hashmap_get(*uid_refs, UID_TO_PTR(uid)));

        n = c & ~DESTROY_IPC_FLAG;
        n++;

        if (n & DESTROY_IPC_FLAG) /* check for overflow */
                return -EOVERFLOW;

        c = n | (c & DESTROY_IPC_FLAG) | (clean_ipc ? DESTROY_IPC_FLAG : 0);

        return hashmap_replace(*uid_refs, UID_TO_PTR(uid), UINT32_TO_PTR(c));
}