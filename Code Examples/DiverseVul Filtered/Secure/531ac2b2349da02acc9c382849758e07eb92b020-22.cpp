static void manager_distribute_fds(Manager *m, FDSet *fds) {
        Iterator i;
        Unit *u;

        assert(m);

        HASHMAP_FOREACH(u, m->units, i) {

                if (fdset_size(fds) <= 0)
                        break;

                if (!UNIT_VTABLE(u)->distribute_fds)
                        continue;

                UNIT_VTABLE(u)->distribute_fds(u, fds);
        }
}