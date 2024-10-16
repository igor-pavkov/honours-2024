pid_t unit_control_pid(Unit *u) {
        assert(u);

        if (UNIT_VTABLE(u)->control_pid)
                return UNIT_VTABLE(u)->control_pid(u);

        return 0;
}