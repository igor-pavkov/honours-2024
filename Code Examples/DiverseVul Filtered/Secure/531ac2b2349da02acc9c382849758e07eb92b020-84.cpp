static void unit_gc_mark_good(Unit *u, unsigned gc_marker) {
        Iterator i;
        Unit *other;

        u->gc_marker = gc_marker + GC_OFFSET_GOOD;

        /* Recursively mark referenced units as GOOD as well */
        SET_FOREACH(other, u->dependencies[UNIT_REFERENCES], i)
                if (other->gc_marker == gc_marker + GC_OFFSET_UNSURE)
                        unit_gc_mark_good(other, gc_marker);
}