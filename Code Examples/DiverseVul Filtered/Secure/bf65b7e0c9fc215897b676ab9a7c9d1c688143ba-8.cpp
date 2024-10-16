bool unit_will_restart(Unit *u) {
        assert(u);

        if (!UNIT_VTABLE(u)->will_restart)
                return false;

        return UNIT_VTABLE(u)->will_restart(u);
}