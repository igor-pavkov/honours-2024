int unit_add_dependency(
                Unit *u,
                UnitDependency d,
                Unit *other,
                bool add_reference,
                UnitDependencyMask mask) {

        static const UnitDependency inverse_table[_UNIT_DEPENDENCY_MAX] = {
                [UNIT_REQUIRES] = UNIT_REQUIRED_BY,
                [UNIT_WANTS] = UNIT_WANTED_BY,
                [UNIT_REQUISITE] = UNIT_REQUISITE_OF,
                [UNIT_BINDS_TO] = UNIT_BOUND_BY,
                [UNIT_PART_OF] = UNIT_CONSISTS_OF,
                [UNIT_REQUIRED_BY] = UNIT_REQUIRES,
                [UNIT_REQUISITE_OF] = UNIT_REQUISITE,
                [UNIT_WANTED_BY] = UNIT_WANTS,
                [UNIT_BOUND_BY] = UNIT_BINDS_TO,
                [UNIT_CONSISTS_OF] = UNIT_PART_OF,
                [UNIT_CONFLICTS] = UNIT_CONFLICTED_BY,
                [UNIT_CONFLICTED_BY] = UNIT_CONFLICTS,
                [UNIT_BEFORE] = UNIT_AFTER,
                [UNIT_AFTER] = UNIT_BEFORE,
                [UNIT_ON_FAILURE] = _UNIT_DEPENDENCY_INVALID,
                [UNIT_REFERENCES] = UNIT_REFERENCED_BY,
                [UNIT_REFERENCED_BY] = UNIT_REFERENCES,
                [UNIT_TRIGGERS] = UNIT_TRIGGERED_BY,
                [UNIT_TRIGGERED_BY] = UNIT_TRIGGERS,
                [UNIT_PROPAGATES_RELOAD_TO] = UNIT_RELOAD_PROPAGATED_FROM,
                [UNIT_RELOAD_PROPAGATED_FROM] = UNIT_PROPAGATES_RELOAD_TO,
                [UNIT_JOINS_NAMESPACE_OF] = UNIT_JOINS_NAMESPACE_OF,
        };
        Unit *original_u = u, *original_other = other;
        int r;

        assert(u);
        assert(d >= 0 && d < _UNIT_DEPENDENCY_MAX);
        assert(other);

        u = unit_follow_merge(u);
        other = unit_follow_merge(other);

        /* We won't allow dependencies on ourselves. We will not
         * consider them an error however. */
        if (u == other) {
                maybe_warn_about_dependency(original_u, original_other->id, d);
                return 0;
        }

        if ((d == UNIT_BEFORE && other->type == UNIT_DEVICE) ||
            (d == UNIT_AFTER && u->type == UNIT_DEVICE)) {
                log_unit_warning(u, "Dependency Before=%s ignored (.device units cannot be delayed)", other->id);
                return 0;
        }

        r = unit_add_dependency_hashmap(u->dependencies + d, other, mask, 0);
        if (r < 0)
                return r;

        if (inverse_table[d] != _UNIT_DEPENDENCY_INVALID && inverse_table[d] != d) {
                r = unit_add_dependency_hashmap(other->dependencies + inverse_table[d], u, 0, mask);
                if (r < 0)
                        return r;
        }

        if (add_reference) {
                r = unit_add_dependency_hashmap(u->dependencies + UNIT_REFERENCES, other, mask, 0);
                if (r < 0)
                        return r;

                r = unit_add_dependency_hashmap(other->dependencies + UNIT_REFERENCED_BY, u, 0, mask);
                if (r < 0)
                        return r;
        }

        unit_add_to_dbus_queue(u);
        return 0;
}