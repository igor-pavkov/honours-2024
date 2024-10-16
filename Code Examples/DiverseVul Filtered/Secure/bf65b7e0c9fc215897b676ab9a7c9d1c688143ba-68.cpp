bool unit_is_pristine(Unit *u) {
        assert(u);

        /* Check if the unit already exists or is already around,
         * in a number of different ways. Note that to cater for unit
         * types such as slice, we are generally fine with units that
         * are marked UNIT_LOADED even though nothing was actually
         * loaded, as those unit types don't require a file on disk. */

        return !(!IN_SET(u->load_state, UNIT_NOT_FOUND, UNIT_LOADED) ||
                 u->fragment_path ||
                 u->source_path ||
                 !strv_isempty(u->dropin_paths) ||
                 u->job ||
                 u->merged_into);
}