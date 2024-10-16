static int merge_names(Unit *u, Unit *other) {
        char *t;
        Iterator i;
        int r;

        assert(u);
        assert(other);

        r = set_complete_move(&u->names, &other->names);
        if (r < 0)
                return r;

        set_free_free(other->names);
        other->names = NULL;
        other->id = NULL;

        SET_FOREACH(t, u->names, i)
                assert_se(hashmap_replace(u->manager->units, t, u) == 0);

        return 0;
}