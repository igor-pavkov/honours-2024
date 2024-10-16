bool unit_has_name(const Unit *u, const char *name) {
        assert(u);
        assert(name);

        return set_contains(u->names, (char*) name);
}