int unit_add_node_dependency(Unit *u, const char *what, bool wants, UnitDependency dep, UnitDependencyMask mask) {
        Unit *device;
        _cleanup_free_ char *e = NULL;
        int r;

        assert(u);

        /* Adds in links to the device node that this unit is based on */
        if (isempty(what))
                return 0;

        if (!is_device_path(what))
                return 0;

        /* When device units aren't supported (such as in a
         * container), don't create dependencies on them. */
        if (!unit_type_supported(UNIT_DEVICE))
                return 0;

        r = unit_name_from_path(what, ".device", &e);
        if (r < 0)
                return r;

        r = manager_load_unit(u->manager, e, NULL, NULL, &device);
        if (r < 0)
                return r;

        if (dep == UNIT_REQUIRES && device_shall_be_bound_by(device, u))
                dep = UNIT_BINDS_TO;

        r = unit_add_two_dependencies(u, UNIT_AFTER,
                                      MANAGER_IS_SYSTEM(u->manager) ? dep : UNIT_WANTS,
                                      device, true, mask);
        if (r < 0)
                return r;

        if (wants) {
                r = unit_add_dependency(device, UNIT_WANTS, u, false, mask);
                if (r < 0)
                        return r;
        }

        return 0;
}