void unit_trigger_notify(Unit *u) {
        Unit *other;
        Iterator i;
        void *v;

        assert(u);

        HASHMAP_FOREACH_KEY(v, other, u->dependencies[UNIT_TRIGGERED_BY], i)
                if (UNIT_VTABLE(other)->trigger_notify)
                        UNIT_VTABLE(other)->trigger_notify(other, u);
}