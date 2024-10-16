void manager_enumerate(Manager *m) {
        UnitType c;

        assert(m);

        /* Let's ask every type to load all units from disk/kernel
         * that it might know */
        for (c = 0; c < _UNIT_TYPE_MAX; c++) {
                if (!unit_type_supported(c)) {
                        log_debug("Unit type .%s is not supported on this system.", unit_type_to_string(c));
                        continue;
                }

                if (!unit_vtable[c]->enumerate)
                        continue;

                unit_vtable[c]->enumerate(m);
        }

        manager_dispatch_load_queue(m);
}