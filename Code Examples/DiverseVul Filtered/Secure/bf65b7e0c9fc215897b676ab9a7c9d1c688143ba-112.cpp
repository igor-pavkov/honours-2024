int unit_acquire_invocation_id(Unit *u) {
        sd_id128_t id;
        int r;

        assert(u);

        r = sd_id128_randomize(&id);
        if (r < 0)
                return log_unit_error_errno(u, r, "Failed to generate invocation ID for unit: %m");

        r = unit_set_invocation_id(u, id);
        if (r < 0)
                return log_unit_error_errno(u, r, "Failed to set invocation ID for unit: %m");

        unit_add_to_dbus_queue(u);
        return 0;
}