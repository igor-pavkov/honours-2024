UnitFileState unit_get_unit_file_state(Unit *u) {
        int r;

        assert(u);

        if (u->unit_file_state < 0 && u->fragment_path) {
                r = unit_file_get_state(
                                u->manager->unit_file_scope,
                                NULL,
                                u->id,
                                &u->unit_file_state);
                if (r < 0)
                        u->unit_file_state = UNIT_FILE_BAD;
        }

        return u->unit_file_state;
}