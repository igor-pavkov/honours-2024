static void bidi_set_free(Unit *u, Hashmap *h) {
        Unit *other;
        Iterator i;
        void *v;

        assert(u);

        /* Frees the hashmap and makes sure we are dropped from the inverse pointers */

        HASHMAP_FOREACH_KEY(v, other, h, i) {
                UnitDependency d;

                for (d = 0; d < _UNIT_DEPENDENCY_MAX; d++)
                        hashmap_remove(other->dependencies[d], u);

                unit_add_to_gc_queue(other);
        }

        hashmap_free(h);
}