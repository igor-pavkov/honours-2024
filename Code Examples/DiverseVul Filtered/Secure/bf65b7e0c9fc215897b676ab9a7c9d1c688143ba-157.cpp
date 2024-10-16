bool unit_can_reload(Unit *u) {
        assert(u);

        if (UNIT_VTABLE(u)->can_reload)
                return UNIT_VTABLE(u)->can_reload(u);

        if (!hashmap_isempty(u->dependencies[UNIT_PROPAGATES_RELOAD_TO]))
                return true;

        return UNIT_VTABLE(u)->reload;
}