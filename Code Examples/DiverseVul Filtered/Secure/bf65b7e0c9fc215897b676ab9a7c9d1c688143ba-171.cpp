const char *unit_slice_name(Unit *u) {
        assert(u);

        if (!UNIT_ISSET(u->slice))
                return NULL;

        return UNIT_DEREF(u->slice)->id;
}