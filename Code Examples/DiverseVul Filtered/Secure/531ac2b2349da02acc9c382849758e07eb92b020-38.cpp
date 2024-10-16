void manager_unref_gid(Manager *m, gid_t gid, bool destroy_now) {
        manager_unref_uid_internal(m, &m->gid_refs, (uid_t) gid, destroy_now, clean_ipc_by_gid);
}