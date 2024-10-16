void manager_vacuum_uid_refs(Manager *m) {
        manager_vacuum_uid_refs_internal(m, &m->uid_refs, clean_ipc_by_uid);
}