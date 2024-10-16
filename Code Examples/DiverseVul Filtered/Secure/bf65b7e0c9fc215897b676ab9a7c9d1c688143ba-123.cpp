void unit_unref_gid(Unit *u, bool destroy_now) {
        unit_unref_uid_internal(u, (uid_t*) &u->ref_gid, destroy_now, manager_unref_gid);
}