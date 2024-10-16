int unit_reload(Unit *u) {
        UnitActiveState state;
        Unit *following;

        assert(u);

        if (u->load_state != UNIT_LOADED)
                return -EINVAL;

        if (!unit_can_reload(u))
                return -EBADR;

        state = unit_active_state(u);
        if (state == UNIT_RELOADING)
                return -EAGAIN;

        if (state != UNIT_ACTIVE) {
                log_unit_warning(u, "Unit cannot be reloaded because it is inactive.");
                return -ENOEXEC;
        }

        following = unit_following(u);
        if (following) {
                log_unit_debug(u, "Redirecting reload request from %s to %s.", u->id, following->id);
                return unit_reload(following);
        }

        unit_add_to_dbus_queue(u);

        if (!UNIT_VTABLE(u)->reload) {
                /* Unit doesn't have a reload function, but we need to propagate the reload anyway */
                unit_notify(u, unit_active_state(u), unit_active_state(u), 0);
                return 0;
        }

        return UNIT_VTABLE(u)->reload(u);
}