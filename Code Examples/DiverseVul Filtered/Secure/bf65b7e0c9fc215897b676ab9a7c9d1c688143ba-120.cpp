void unit_unref_uid_gid(Unit *u, bool destroy_now) {
        assert(u);

        unit_unref_uid(u, destroy_now);
        unit_unref_gid(u, destroy_now);
}