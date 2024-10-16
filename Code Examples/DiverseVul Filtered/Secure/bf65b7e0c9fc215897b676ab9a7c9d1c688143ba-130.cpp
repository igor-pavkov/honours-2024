static void unit_update_dependency_mask(Unit *u, UnitDependency d, Unit *other, UnitDependencyInfo di) {
        assert(u);
        assert(d >= 0);
        assert(d < _UNIT_DEPENDENCY_MAX);
        assert(other);

        if (di.origin_mask == 0 && di.destination_mask == 0) {
                /* No bit set anymore, let's drop the whole entry */
                assert_se(hashmap_remove(u->dependencies[d], other));
                log_unit_debug(u, "%s lost dependency %s=%s", u->id, unit_dependency_to_string(d), other->id);
        } else
                /* Mask was reduced, let's update the entry */
                assert_se(hashmap_update(u->dependencies[d], other, di.data) == 0);
}