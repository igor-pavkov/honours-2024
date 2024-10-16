void unit_remove_dependencies(Unit *u, UnitDependencyMask mask) {
        UnitDependency d;

        assert(u);

        /* Removes all dependencies u has on other units marked for ownership by 'mask'. */

        if (mask == 0)
                return;

        for (d = 0; d < _UNIT_DEPENDENCY_MAX; d++) {
                bool done;

                do {
                        UnitDependencyInfo di;
                        Unit *other;
                        Iterator i;

                        done = true;

                        HASHMAP_FOREACH_KEY(di.data, other, u->dependencies[d], i) {
                                UnitDependency q;

                                if ((di.origin_mask & ~mask) == di.origin_mask)
                                        continue;
                                di.origin_mask &= ~mask;
                                unit_update_dependency_mask(u, d, other, di);

                                /* We updated the dependency from our unit to the other unit now. But most dependencies
                                 * imply a reverse dependency. Hence, let's delete that one too. For that we go through
                                 * all dependency types on the other unit and delete all those which point to us and
                                 * have the right mask set. */

                                for (q = 0; q < _UNIT_DEPENDENCY_MAX; q++) {
                                        UnitDependencyInfo dj;

                                        dj.data = hashmap_get(other->dependencies[q], u);
                                        if ((dj.destination_mask & ~mask) == dj.destination_mask)
                                                continue;
                                        dj.destination_mask &= ~mask;

                                        unit_update_dependency_mask(other, q, u, dj);
                                }

                                unit_add_to_gc_queue(other);

                                done = false;
                                break;
                        }

                } while (!done);
        }
}