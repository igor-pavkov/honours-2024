static int unit_add_mount_dependencies(Unit *u) {
        UnitDependencyInfo di;
        const char *path;
        Iterator i;
        int r;

        assert(u);

        HASHMAP_FOREACH_KEY(di.data, path, u->requires_mounts_for, i) {
                char prefix[strlen(path) + 1];

                PATH_FOREACH_PREFIX_MORE(prefix, path) {
                        _cleanup_free_ char *p = NULL;
                        Unit *m;

                        r = unit_name_from_path(prefix, ".mount", &p);
                        if (r < 0)
                                return r;

                        m = manager_get_unit(u->manager, p);
                        if (!m) {
                                /* Make sure to load the mount unit if
                                 * it exists. If so the dependencies
                                 * on this unit will be added later
                                 * during the loading of the mount
                                 * unit. */
                                (void) manager_load_unit_prepare(u->manager, p, NULL, NULL, &m);
                                continue;
                        }
                        if (m == u)
                                continue;

                        if (m->load_state != UNIT_LOADED)
                                continue;

                        r = unit_add_dependency(u, UNIT_AFTER, m, true, di.origin_mask);
                        if (r < 0)
                                return r;

                        if (m->fragment_path) {
                                r = unit_add_dependency(u, UNIT_REQUIRES, m, true, di.origin_mask);
                                if (r < 0)
                                        return r;
                        }
                }
        }

        return 0;
}