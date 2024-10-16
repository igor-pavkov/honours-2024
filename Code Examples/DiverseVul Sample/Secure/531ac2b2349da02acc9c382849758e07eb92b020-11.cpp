static void manager_serialize_uid_refs_internal(
                Manager *m,
                FILE *f,
                Hashmap **uid_refs,
                const char *field_name) {

        Iterator i;
        void *p, *k;

        assert(m);
        assert(f);
        assert(uid_refs);
        assert(field_name);

        /* Serialize the UID reference table. Or actually, just the IPC destruction flag of it, as the actual counter
         * of it is better rebuild after a reload/reexec. */

        HASHMAP_FOREACH_KEY(p, k, *uid_refs, i) {
                uint32_t c;
                uid_t uid;

                uid = PTR_TO_UID(k);
                c = PTR_TO_UINT32(p);

                if (!(c & DESTROY_IPC_FLAG))
                        continue;

                fprintf(f, "%s=" UID_FMT "\n", field_name, uid);
        }
}