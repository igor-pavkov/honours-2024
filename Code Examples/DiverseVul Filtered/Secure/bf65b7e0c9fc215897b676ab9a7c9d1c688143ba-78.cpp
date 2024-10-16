Unit* unit_ref_set(UnitRef *ref, Unit *source, Unit *target) {
        assert(ref);
        assert(source);
        assert(target);

        if (ref->target)
                unit_ref_unset(ref);

        ref->source = source;
        ref->target = target;
        LIST_PREPEND(refs_by_target, target->refs_by_target, ref);
        return target;
}