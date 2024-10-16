bool unit_can_serialize(Unit *u) {
        assert(u);

        return UNIT_VTABLE(u)->serialize && UNIT_VTABLE(u)->deserialize_item;
}