void unit_notify_user_lookup(Unit *u, uid_t uid, gid_t gid) {
        int r;

        assert(u);

        /* This is invoked whenever one of the forked off processes let's us know the UID/GID its user name/group names
         * resolved to. We keep track of which UID/GID is currently assigned in order to be able to destroy its IPC
         * objects when no service references the UID/GID anymore. */

        r = unit_ref_uid_gid(u, uid, gid);
        if (r > 0)
                unit_add_to_dbus_queue(u);
}