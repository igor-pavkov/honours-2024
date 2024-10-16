static void unit_free_requires_mounts_for(Unit *u) {
        assert(u);

        for (;;) {
                _cleanup_free_ char *path;

                path = hashmap_steal_first_key(u->requires_mounts_for);
                if (!path)
                        break;
                else {
                        char s[strlen(path) + 1];

                        PATH_FOREACH_PREFIX_MORE(s, path) {
                                char *y;
                                Set *x;

                                x = hashmap_get2(u->manager->units_requiring_mounts_for, s, (void**) &y);
                                if (!x)
                                        continue;

                                (void) set_remove(x, u);

                                if (set_isempty(x)) {
                                        (void) hashmap_remove(u->manager->units_requiring_mounts_for, y);
                                        free(y);
                                        set_free(x);
                                }
                        }
                }
        }

        u->requires_mounts_for = hashmap_free(u->requires_mounts_for);
}