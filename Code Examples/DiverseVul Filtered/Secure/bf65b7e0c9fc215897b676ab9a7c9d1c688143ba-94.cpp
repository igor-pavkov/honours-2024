static bool unit_test_assert(Unit *u) {
        assert(u);

        dual_timestamp_get(&u->assert_timestamp);
        u->assert_result = condition_test_list(u->asserts, assert_type_to_string, log_unit_internal, u);

        unit_add_to_dbus_queue(u);

        return u->assert_result;
}