void manager_unref_uid(Manager *m, uid_t uid, bool destroy_now) {
        manager_unref_uid_internal(m, &m->uid_refs, uid, destroy_now, clean_ipc_by_uid);
}