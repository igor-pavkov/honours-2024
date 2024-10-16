bool unit_can_isolate(Unit *u) {
        assert(u);

        return unit_can_start(u) &&
                u->allow_isolate;
}