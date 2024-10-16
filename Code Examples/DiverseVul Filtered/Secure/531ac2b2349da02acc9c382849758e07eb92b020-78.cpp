int manager_ref_uid(Manager *m, uid_t uid, bool clean_ipc) {
        return manager_ref_uid_internal(m, &m->uid_refs, uid, clean_ipc);
}