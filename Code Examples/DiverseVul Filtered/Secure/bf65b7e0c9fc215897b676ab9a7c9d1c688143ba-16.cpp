void unit_add_to_load_queue(Unit *u) {
        assert(u);
        assert(u->type != _UNIT_TYPE_INVALID);

        if (u->load_state != UNIT_STUB || u->in_load_queue)
                return;

        LIST_PREPEND(load_queue, u->manager->load_queue, u);
        u->in_load_queue = true;
}