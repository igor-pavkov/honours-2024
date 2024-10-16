void unit_add_to_target_deps_queue(Unit *u) {
        Manager *m = u->manager;

        assert(u);

        if (u->in_target_deps_queue)
                return;

        LIST_PREPEND(target_deps_queue, m->target_deps_queue, u);
        u->in_target_deps_queue = true;
}