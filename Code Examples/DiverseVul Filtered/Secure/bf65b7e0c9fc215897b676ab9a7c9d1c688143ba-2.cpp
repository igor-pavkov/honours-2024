static bool unit_test_condition(Unit *u) {
        assert(u);

        dual_timestamp_get(&u->condition_timestamp);
        u->condition_result = condition_test_list(u->conditions, condition_type_to_string, log_unit_internal, u);

        unit_add_to_dbus_queue(u);

        return u->condition_result;
}