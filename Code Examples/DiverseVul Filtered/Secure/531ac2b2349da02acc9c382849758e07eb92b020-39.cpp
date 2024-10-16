int manager_load_unit_prepare(
                Manager *m,
                const char *name,
                const char *path,
                sd_bus_error *e,
                Unit **_ret) {

        Unit *ret;
        UnitType t;
        int r;

        assert(m);
        assert(name || path);

        /* This will prepare the unit for loading, but not actually
         * load anything from disk. */

        if (path && !is_path(path))
                return sd_bus_error_setf(e, SD_BUS_ERROR_INVALID_ARGS, "Path %s is not absolute.", path);

        if (!name)
                name = basename(path);

        t = unit_name_to_type(name);

        if (t == _UNIT_TYPE_INVALID || !unit_name_is_valid(name, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE)) {
                if (unit_name_is_valid(name, UNIT_NAME_TEMPLATE))
                        return sd_bus_error_setf(e, SD_BUS_ERROR_INVALID_ARGS, "Unit name %s is missing the instance name.", name);

                return sd_bus_error_setf(e, SD_BUS_ERROR_INVALID_ARGS, "Unit name %s is not valid.", name);
        }

        ret = manager_get_unit(m, name);
        if (ret) {
                *_ret = ret;
                return 1;
        }

        ret = unit_new(m, unit_vtable[t]->object_size);
        if (!ret)
                return -ENOMEM;

        if (path) {
                ret->fragment_path = strdup(path);
                if (!ret->fragment_path) {
                        unit_free(ret);
                        return -ENOMEM;
                }
        }

        r = unit_add_name(ret, name);
        if (r < 0) {
                unit_free(ret);
                return r;
        }

        unit_add_to_load_queue(ret);
        unit_add_to_dbus_queue(ret);
        unit_add_to_gc_queue(ret);

        if (_ret)
                *_ret = ret;

        return 0;
}