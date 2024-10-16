void manager_deserialize_gid_refs_one(Manager *m, const char *value) {
        manager_deserialize_uid_refs_one_internal(m, &m->gid_refs, value);
}