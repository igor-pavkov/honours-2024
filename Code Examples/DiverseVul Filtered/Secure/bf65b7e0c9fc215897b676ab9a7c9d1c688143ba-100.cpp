static void merge_dependencies(Unit *u, Unit *other, const char *other_id, UnitDependency d) {
        Iterator i;
        Unit *back;
        void *v;
        int r;

        /* Merges all dependencies of type 'd' of the unit 'other' into the deps of the unit 'u' */

        assert(u);
        assert(other);
        assert(d < _UNIT_DEPENDENCY_MAX);

        /* Fix backwards pointers. Let's iterate through all dependendent units of the other unit. */
        HASHMAP_FOREACH_KEY(v, back, other->dependencies[d], i) {
                UnitDependency k;

                /* Let's now iterate through the dependencies of that dependencies of the other units, looking for
                 * pointers back, and let's fix them up, to instead point to 'u'. */

                for (k = 0; k < _UNIT_DEPENDENCY_MAX; k++) {
                        if (back == u) {
                                /* Do not add dependencies between u and itself. */
                                if (hashmap_remove(back->dependencies[k], other))
                                        maybe_warn_about_dependency(u, other_id, k);
                        } else {
                                UnitDependencyInfo di_u, di_other, di_merged;

                                /* Let's drop this dependency between "back" and "other", and let's create it between
                                 * "back" and "u" instead. Let's merge the bit masks of the dependency we are moving,
                                 * and any such dependency which might already exist */

                                di_other.data = hashmap_get(back->dependencies[k], other);
                                if (!di_other.data)
                                        continue; /* dependency isn't set, let's try the next one */

                                di_u.data = hashmap_get(back->dependencies[k], u);

                                di_merged = (UnitDependencyInfo) {
                                        .origin_mask = di_u.origin_mask | di_other.origin_mask,
                                        .destination_mask = di_u.destination_mask | di_other.destination_mask,
                                };

                                r = hashmap_remove_and_replace(back->dependencies[k], other, u, di_merged.data);
                                if (r < 0)
                                        log_warning_errno(r, "Failed to remove/replace: back=%s other=%s u=%s: %m", back->id, other_id, u->id);
                                assert(r >= 0);

                                /* assert_se(hashmap_remove_and_replace(back->dependencies[k], other, u, di_merged.data) >= 0); */
                        }
                }

        }

        /* Also do not move dependencies on u to itself */
        back = hashmap_remove(other->dependencies[d], u);
        if (back)
                maybe_warn_about_dependency(u, other_id, d);

        /* The move cannot fail. The caller must have performed a reservation. */
        assert_se(hashmap_complete_move(&u->dependencies[d], &other->dependencies[d]) == 0);

        other->dependencies[d] = hashmap_free(other->dependencies[d]);
}