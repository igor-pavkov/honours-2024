int unit_get_unit_file_preset(Unit *u) {
        assert(u);

        if (u->unit_file_preset < 0 && u->fragment_path)
                u->unit_file_preset = unit_file_query_preset(
                                u->manager->unit_file_scope,
                                NULL,
                                basename(u->fragment_path));

        return u->unit_file_preset;
}