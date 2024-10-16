int unit_merge(Unit *u, Unit *other) {
        UnitDependency d;
        const char *other_id = NULL;
        int r;

        assert(u);
        assert(other);
        assert(u->manager == other->manager);
        assert(u->type != _UNIT_TYPE_INVALID);

        other = unit_follow_merge(other);

        if (other == u)
                return 0;

        if (u->type != other->type)
                return -EINVAL;

        if (!u->instance != !other->instance)
                return -EINVAL;

        if (!unit_type_may_alias(u->type)) /* Merging only applies to unit names that support aliases */
                return -EEXIST;

        if (!IN_SET(other->load_state, UNIT_STUB, UNIT_NOT_FOUND))
                return -EEXIST;

        if (other->job)
                return -EEXIST;

        if (other->nop_job)
                return -EEXIST;

        if (!UNIT_IS_INACTIVE_OR_FAILED(unit_active_state(other)))
                return -EEXIST;

        if (other->id)
                other_id = strdupa(other->id);

        /* Make reservations to ensure merge_dependencies() won't fail */
        for (d = 0; d < _UNIT_DEPENDENCY_MAX; d++) {
                r = reserve_dependencies(u, other, d);
                /*
                 * We don't rollback reservations if we fail. We don't have
                 * a way to undo reservations. A reservation is not a leak.
                 */
                if (r < 0)
                        return r;
        }

        /* Merge names */
        r = merge_names(u, other);
        if (r < 0)
                return r;

        /* Redirect all references */
        while (other->refs_by_target)
                unit_ref_set(other->refs_by_target, other->refs_by_target->source, u);

        /* Merge dependencies */
        for (d = 0; d < _UNIT_DEPENDENCY_MAX; d++)
                merge_dependencies(u, other, other_id, d);

        other->load_state = UNIT_MERGED;
        other->merged_into = u;

        /* If there is still some data attached to the other node, we
         * don't need it anymore, and can free it. */
        if (other->load_state != UNIT_STUB)
                if (UNIT_VTABLE(other)->done)
                        UNIT_VTABLE(other)->done(other);

        unit_add_to_dbus_queue(u);
        unit_add_to_cleanup_queue(other);

        return 0;
}