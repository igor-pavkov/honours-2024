void unit_submit_to_stop_when_unneeded_queue(Unit *u) {
        assert(u);

        if (u->in_stop_when_unneeded_queue)
                return;

        if (!u->stop_when_unneeded)
                return;

        if (!UNIT_IS_ACTIVE_OR_RELOADING(unit_active_state(u)))
                return;

        LIST_PREPEND(stop_when_unneeded_queue, u->manager->stop_when_unneeded_queue, u);
        u->in_stop_when_unneeded_queue = true;
}