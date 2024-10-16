static int property_read_restrict_address_families(
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

        info->restrict_address_family_inet =
                info->restrict_address_family_unix =
                info->restrict_address_family_netlink =
                info->restrict_address_family_packet =
                info->restrict_address_family_other = whitelist;

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

                if (STR_IN_SET(name, "AF_INET", "AF_INET6"))
                        info->restrict_address_family_inet = !whitelist;
                else if (streq(name, "AF_UNIX"))
                        info->restrict_address_family_unix = !whitelist;
                else if (streq(name, "AF_NETLINK"))
                        info->restrict_address_family_netlink = !whitelist;
                else if (streq(name, "AF_PACKET"))
                        info->restrict_address_family_packet = !whitelist;
                else
                        info->restrict_address_family_other = !whitelist;
        }

        r = sd_bus_message_exit_container(m);
        if (r < 0)
                return r;

        return sd_bus_message_exit_container(m);
}