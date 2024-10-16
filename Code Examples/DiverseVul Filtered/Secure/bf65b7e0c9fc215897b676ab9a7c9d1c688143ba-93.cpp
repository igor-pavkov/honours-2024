static int reserve_dependencies(Unit *u, Unit *other, UnitDependency d) {
        unsigned n_reserve;

        assert(u);
        assert(other);
        assert(d < _UNIT_DEPENDENCY_MAX);

        /*
         * If u does not have this dependency set allocated, there is no need
         * to reserve anything. In that case other's set will be transferred
         * as a whole to u by complete_move().
         */
        if (!u->dependencies[d])
                return 0;

        /* merge_dependencies() will skip a u-on-u dependency */
        n_reserve = hashmap_size(other->dependencies[d]) - !!hashmap_get(other->dependencies[d], u);

        return hashmap_reserve(u->dependencies[d], n_reserve);
}