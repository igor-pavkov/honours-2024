int unit_set_default_slice(Unit *u) {
        const char *slice_name;
        Unit *slice;
        int r;

        assert(u);

        if (UNIT_ISSET(u->slice))
                return 0;

        if (u->instance) {
                _cleanup_free_ char *prefix = NULL, *escaped = NULL;

                /* Implicitly place all instantiated units in their
                 * own per-template slice */

                r = unit_name_to_prefix(u->id, &prefix);
                if (r < 0)
                        return r;

                /* The prefix is already escaped, but it might include
                 * "-" which has a special meaning for slice units,
                 * hence escape it here extra. */
                escaped = unit_name_escape(prefix);
                if (!escaped)
                        return -ENOMEM;

                if (MANAGER_IS_SYSTEM(u->manager))
                        slice_name = strjoina("system-", escaped, ".slice");
                else
                        slice_name = strjoina(escaped, ".slice");
        } else
                slice_name =
                        MANAGER_IS_SYSTEM(u->manager) && !unit_has_name(u, SPECIAL_INIT_SCOPE)
                        ? SPECIAL_SYSTEM_SLICE
                        : SPECIAL_ROOT_SLICE;

        r = manager_load_unit(u->manager, slice_name, NULL, NULL, &slice);
        if (r < 0)
                return r;

        return unit_set_slice(u, slice);
}