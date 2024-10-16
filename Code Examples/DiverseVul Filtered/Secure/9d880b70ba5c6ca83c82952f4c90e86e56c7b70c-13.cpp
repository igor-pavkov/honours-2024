static int property_read_device_allow(
                sd_bus *bus,
                const char *member,
                sd_bus_message *m,
                sd_bus_error *error,
                void *userdata) {

        struct security_info *info = userdata;
        size_t n = 0;
        int r;

        assert(bus);
        assert(member);
        assert(m);

        r = sd_bus_message_enter_container(m, 'a', "(ss)");
        if (r < 0)
                return r;

        for (;;) {
                const char *name, *policy;

                r = sd_bus_message_read(m, "(ss)", &name, &policy);
                if (r < 0)
                        return r;
                if (r == 0)
                        break;

                n++;
        }

        info->device_allow_non_empty = n > 0;

        return sd_bus_message_exit_container(m);
}