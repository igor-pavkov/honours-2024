bool unit_can_start(Unit *u) {
        assert(u);

        if (u->load_state != UNIT_LOADED)
                return false;

        if (!unit_supported(u))
                return false;

        /* Scope units may be started only once */
        if (UNIT_VTABLE(u)->once_only && dual_timestamp_is_set(&u->inactive_exit_timestamp))
                return false;

        return !!UNIT_VTABLE(u)->start;
}