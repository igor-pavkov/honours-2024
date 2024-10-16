pid_t unit_main_pid(Unit *u) {
        assert(u);

        if (UNIT_VTABLE(u)->main_pid)
                return UNIT_VTABLE(u)->main_pid(u);

        return 0;
}