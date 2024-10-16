const char *unit_description(Unit *u) {
        assert(u);

        if (u->description)
                return u->description;

        return strna(u->id);
}