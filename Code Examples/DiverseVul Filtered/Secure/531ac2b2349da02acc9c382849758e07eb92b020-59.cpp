int manager_ref_gid(Manager *m, gid_t gid, bool clean_ipc) {
        return manager_ref_uid_internal(m, &m->gid_refs, (uid_t) gid, clean_ipc);
}