int unit_require_mounts_for(Unit *u, const char *path, UnitDependencyMask mask) {
        _cleanup_free_ char *p = NULL;
        UnitDependencyInfo di;
        int r;

        assert(u);
        assert(path);

        /* Registers a unit for requiring a certain path and all its prefixes. We keep a hashtable of these paths in
         * the unit (from the path to the UnitDependencyInfo structure indicating how to the dependency came to
         * be). However, we build a prefix table for all possible prefixes so that new appearing mount units can easily
         * determine which units to make themselves a dependency of. */

        if (!path_is_absolute(path))
                return -EINVAL;

        r = hashmap_ensure_allocated(&u->requires_mounts_for, &path_hash_ops);
        if (r < 0)
                return r;

        p = strdup(path);
        if (!p)
                return -ENOMEM;

        path = path_simplify(p, true);

        if (!path_is_normalized(path))
                return -EPERM;

        if (hashmap_contains(u->requires_mounts_for, path))
                return 0;

        di = (UnitDependencyInfo) {
                .origin_mask = mask
        };

        r = hashmap_put(u->requires_mounts_for, path, di.data);
        if (r < 0)
                return r;
        p = NULL;

        char prefix[strlen(path) + 1];
        PATH_FOREACH_PREFIX_MORE(prefix, path) {
                Set *x;

                x = hashmap_get(u->manager->units_requiring_mounts_for, prefix);
                if (!x) {
                        _cleanup_free_ char *q = NULL;

                        r = hashmap_ensure_allocated(&u->manager->units_requiring_mounts_for, &path_hash_ops);
                        if (r < 0)
                                return r;

                        q = strdup(prefix);
                        if (!q)
                                return -ENOMEM;

                        x = set_new(NULL);
                        if (!x)
                                return -ENOMEM;

                        r = hashmap_put(u->manager->units_requiring_mounts_for, q, x);
                        if (r < 0) {
                                set_free(x);
                                return r;
                        }
                        q = NULL;
                }

                r = set_put(x, u);
                if (r < 0)
                        return r;
        }

        return 0;
}