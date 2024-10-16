void unit_ref_unset(UnitRef *ref) {
        assert(ref);

        if (!ref->target)
                return;

        /* We are about to drop a reference to the unit, make sure the garbage collection has a look at it as it might
         * be unreferenced now. */
        unit_add_to_gc_queue(ref->target);

        LIST_REMOVE(refs_by_target, ref->target->refs_by_target, ref);
        ref->source = ref->target = NULL;
}