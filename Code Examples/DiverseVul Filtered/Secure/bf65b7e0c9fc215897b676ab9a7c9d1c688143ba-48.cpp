bool unit_needs_console(Unit *u) {
        ExecContext *ec;
        UnitActiveState state;

        assert(u);

        state = unit_active_state(u);

        if (UNIT_IS_INACTIVE_OR_FAILED(state))
                return false;

        if (UNIT_VTABLE(u)->needs_console)
                return UNIT_VTABLE(u)->needs_console(u);

        /* If this unit type doesn't implement this call, let's use a generic fallback implementation: */
        ec = unit_get_exec_context(u);
        if (!ec)
                return false;

        return exec_context_may_touch_console(ec);
}