void manager_serialize_gid_refs(Manager *m, FILE *f) {
        manager_serialize_uid_refs_internal(m, f, &m->gid_refs, "destroy-ipc-gid");
}