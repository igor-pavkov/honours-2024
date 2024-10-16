void unit_catchup(Unit *u) {
        assert(u);

        if (UNIT_VTABLE(u)->catchup)
                UNIT_VTABLE(u)->catchup(u);
}