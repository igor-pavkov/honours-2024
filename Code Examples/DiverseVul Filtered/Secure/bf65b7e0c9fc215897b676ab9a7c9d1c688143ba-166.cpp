void unit_add_to_dbus_queue(Unit *u) {
        assert(u);
        assert(u->type != _UNIT_TYPE_INVALID);

        if (u->load_state == UNIT_STUB || u->in_dbus_queue)
                return;

        /* Shortcut things if nobody cares */
        if (sd_bus_track_count(u->manager->subscribed) <= 0 &&
            sd_bus_track_count(u->bus_track) <= 0 &&
            set_isempty(u->manager->private_buses)) {
                u->sent_dbus_new_signal = true;
                return;
        }

        LIST_PREPEND(dbus_queue, u->manager->dbus_unit_queue, u);
        u->in_dbus_queue = true;
}