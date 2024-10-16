bool unit_can_stop(Unit *u) {
        assert(u);

        if (!unit_supported(u))
                return false;

        if (u->perpetual)
                return false;

        return !!UNIT_VTABLE(u)->stop;
}