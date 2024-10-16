static int property_read_ip_address_allow(
                sd_bus *bus,
                const char *member,
                sd_bus_message *m,
                sd_bus_error *error,
                void *userdata) {

        struct security_info *info = userdata;
        bool deny_ipv4 = false, deny_ipv6 = false;
        int r;

        assert(bus);
        assert(member);
        assert(m);

        r = sd_bus_message_enter_container(m, 'a', "(iayu)");
        if (r < 0)
                return r;

        for (;;) {
                const void *data;
                size_t size;
                int32_t family;
                uint32_t prefixlen;

                r = sd_bus_message_enter_container(m, 'r', "iayu");
                if (r < 0)
                        return r;
                if (r == 0)
                        break;

                r = sd_bus_message_read(m, "i", &family);
                if (r < 0)
                        return r;

                r = sd_bus_message_read_array(m, 'y', &data, &size);
                if (r < 0)
                        return r;

                r = sd_bus_message_read(m, "u", &prefixlen);
                if (r < 0)
                        return r;

                r = sd_bus_message_exit_container(m);
                if (r < 0)
                        return r;

                if (streq(member, "IPAddressAllow")) {
                        union in_addr_union u;

                        if (family == AF_INET && size == 4 && prefixlen == 8)
                                memcpy(&u.in, data, size);
                        else if (family == AF_INET6 && size == 16 && prefixlen == 128)
                                memcpy(&u.in6, data, size);
                        else {
                                info->ip_address_allow_other = true;
                                continue;
                        }

                        if (in_addr_is_localhost(family, &u))
                                info->ip_address_allow_localhost = true;
                        else
                                info->ip_address_allow_other = true;
                } else {
                        assert(streq(member, "IPAddressDeny"));

                        if (family == AF_INET && size == 4 && prefixlen == 0)
                                deny_ipv4 = true;
                        else if (family == AF_INET6 && size == 16 && prefixlen == 0)
                                deny_ipv6 = true;
                }
        }

        info->ip_address_deny_all = deny_ipv4 && deny_ipv6;

        return sd_bus_message_exit_container(m);
}