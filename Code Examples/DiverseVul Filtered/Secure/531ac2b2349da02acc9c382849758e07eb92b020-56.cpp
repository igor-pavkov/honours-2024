static void manager_coldplug(Manager *m) {
        Iterator i;
        Unit *u;
        char *k;
        int r;

        assert(m);

        /* Then, let's set up their initial state. */
        HASHMAP_FOREACH_KEY(u, k, m->units, i) {

                /* ignore aliases */
                if (u->id != k)
                        continue;

                r = unit_coldplug(u);
                if (r < 0)
                        log_warning_errno(r, "We couldn't coldplug %s, proceeding anyway: %m", u->id);
        }
}