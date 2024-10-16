int analyze_security(sd_bus *bus, char **units, AnalyzeSecurityFlags flags) {
        _cleanup_(table_unrefp) Table *overview_table = NULL;
        int ret = 0, r;

        assert(bus);

        if (strv_length(units) != 1) {
                overview_table = table_new("unit", "exposure", "predicate", "happy");
                if (!overview_table)
                        return log_oom();
        }

        if (strv_isempty(units)) {
                _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
                _cleanup_(sd_bus_message_unrefp) sd_bus_message *reply = NULL;
                _cleanup_strv_free_ char **list = NULL;
                size_t allocated = 0, n = 0;
                char **i;

                r = sd_bus_call_method(
                                bus,
                                "org.freedesktop.systemd1",
                                "/org/freedesktop/systemd1",
                                "org.freedesktop.systemd1.Manager",
                                "ListUnits",
                                &error, &reply,
                                NULL);
                if (r < 0)
                        return log_error_errno(r, "Failed to list units: %s", bus_error_message(&error, r));

                r = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "(ssssssouso)");
                if (r < 0)
                        return bus_log_parse_error(r);

                for (;;) {
                        UnitInfo info;
                        char *copy = NULL;

                        r = bus_parse_unit_info(reply, &info);
                        if (r < 0)
                                return bus_log_parse_error(r);
                        if (r == 0)
                                break;

                        if (!endswith(info.id, ".service"))
                                continue;

                        if (!GREEDY_REALLOC(list, allocated, n+2))
                                return log_oom();

                        copy = strdup(info.id);
                        if (!copy)
                                return log_oom();

                        list[n++] = copy;
                        list[n] = NULL;
                }

                strv_sort(list);

                flags |= ANALYZE_SECURITY_SHORT|ANALYZE_SECURITY_ONLY_LOADED|ANALYZE_SECURITY_ONLY_LONG_RUNNING;

                STRV_FOREACH(i, list) {
                        r = analyze_security_one(bus, *i, overview_table, flags);
                        if (r < 0 && ret >= 0)
                                ret = r;
                }

        } else {
                char **i;

                STRV_FOREACH(i, units) {
                        _cleanup_free_ char *mangled = NULL, *instance = NULL;
                        const char *name;

                        if (!FLAGS_SET(flags, ANALYZE_SECURITY_SHORT) && i != units) {
                                putc('\n', stdout);
                                fflush(stdout);
                        }

                        r = unit_name_mangle_with_suffix(*i, 0, ".service", &mangled);
                        if (r < 0)
                                return log_error_errno(r, "Failed to mangle unit name '%s': %m", *i);

                        if (!endswith(mangled, ".service")) {
                                log_error("Unit %s is not a service unit, refusing.", *i);
                                return -EINVAL;
                        }

                        if (unit_name_is_valid(mangled, UNIT_NAME_TEMPLATE)) {
                                r = unit_name_replace_instance(mangled, "test-instance", &instance);
                                if (r < 0)
                                        return log_oom();

                                name = instance;
                        } else
                                name = mangled;

                        r = analyze_security_one(bus, name, overview_table, flags);
                        if (r < 0 && ret >= 0)
                                ret = r;
                }
        }

        if (overview_table) {
                if (!FLAGS_SET(flags, ANALYZE_SECURITY_SHORT)) {
                        putc('\n', stdout);
                        fflush(stdout);
                }

                r = table_print(overview_table, stdout);
                if (r < 0)
                        return log_error_errno(r, "Failed to output table: %m");
        }

        return ret;
}