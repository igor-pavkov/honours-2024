void manager_serialize_uid_refs(Manager *m, FILE *f) {
        manager_serialize_uid_refs_internal(m, f, &m->uid_refs, "destroy-ipc-uid");
}