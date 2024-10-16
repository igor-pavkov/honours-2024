void unit_reset_failed(Unit *u) {
        assert(u);

        if (UNIT_VTABLE(u)->reset_failed)
                UNIT_VTABLE(u)->reset_failed(u);

        RATELIMIT_RESET(u->start_limit);
        u->start_limit_hit = false;
}