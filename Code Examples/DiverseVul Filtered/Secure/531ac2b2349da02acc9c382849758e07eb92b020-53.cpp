static void manager_deserialize_uid_refs_one_internal(
                Manager *m,
                Hashmap** uid_refs,
                const char *value) {

        uid_t uid;
        uint32_t c;
        int r;

        assert(m);
        assert(uid_refs);
        assert(value);

        r = parse_uid(value, &uid);
        if (r < 0 || uid == 0) {
                log_debug("Unable to parse UID reference serialization");
                return;
        }

        r = hashmap_ensure_allocated(uid_refs, &trivial_hash_ops);
        if (r < 0) {
                log_oom();
                return;
        }

        c = PTR_TO_UINT32(hashmap_get(*uid_refs, UID_TO_PTR(uid)));
        if (c & DESTROY_IPC_FLAG)
                return;

        c |= DESTROY_IPC_FLAG;

        r = hashmap_replace(*uid_refs, UID_TO_PTR(uid), UINT32_TO_PTR(c));
        if (r < 0) {
                log_debug("Failed to add UID reference entry");
                return;
        }
}