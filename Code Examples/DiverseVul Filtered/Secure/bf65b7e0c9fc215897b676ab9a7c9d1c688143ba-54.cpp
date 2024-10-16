int unit_test_trigger_loaded(Unit *u) {
        Unit *trigger;

        /* Tests whether the unit to trigger is loaded */

        trigger = UNIT_TRIGGER(u);
        if (!trigger)
                return log_unit_error_errno(u, SYNTHETIC_ERRNO(ENOENT), "Refusing to start, unit to trigger not loaded.");
        if (trigger->load_state != UNIT_LOADED)
                return log_unit_error_errno(u, SYNTHETIC_ERRNO(ENOENT), "Refusing to start, unit %s to trigger not loaded.", u->id);

        return 0;
}