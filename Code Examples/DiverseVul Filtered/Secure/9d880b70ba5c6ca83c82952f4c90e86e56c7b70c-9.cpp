static int property_read_system_call_filter(
                sd_bus *bus,
                const char *member,
                sd_bus_message *m,
                sd_bus_error *error,
                void *userdata) {

        struct security_info *info = userdata;
        int whitelist, r;

        assert(bus);
        assert(member);
        assert(m);

        r = sd_bus_message_enter_container(m, 'r', "bas");
        if (r < 0)
                return r;

        r = sd_bus_message_read(m, "b", &whitelist);
        if (r < 0)
                return r;

        info->system_call_filter_whitelist = whitelist;

        r = sd_bus_message_enter_container(m, 'a', "s");
        if (r < 0)
                return r;

        for (;;) {
                const char *name;

                r = sd_bus_message_read(m, "s", &name);
                if (r < 0)
                        return r;
                if (r == 0)
                        break;

                r = set_ensure_allocated(&info->system_call_filter, &string_hash_ops);
                if (r < 0)
                        return r;

                r = set_put_strdup(info->system_call_filter, name);
                if (r < 0)
                        return r;
        }

        r = sd_bus_message_exit_container(m);
        if (r < 0)
                return r;

        return sd_bus_message_exit_container(m);
}