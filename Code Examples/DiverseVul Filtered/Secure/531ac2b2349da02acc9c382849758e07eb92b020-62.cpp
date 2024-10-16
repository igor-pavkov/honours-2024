void manager_vacuum_gid_refs(Manager *m) {
        manager_vacuum_uid_refs_internal(m, &m->gid_refs, clean_ipc_by_gid);
}