static int dhcp6_option_parse_address(DHCP6Option *option, DHCP6IA *ia,
                                      uint32_t *lifetime_valid) {
        DHCP6AddressOption *addr_option = (DHCP6AddressOption *)option;
        DHCP6Address *addr;
        uint32_t lt_valid, lt_pref;
        int r;

        if (be16toh(option->len) + sizeof(DHCP6Option) < sizeof(*addr_option))
                return -ENOBUFS;

        lt_valid = be32toh(addr_option->iaaddr.lifetime_valid);
        lt_pref = be32toh(addr_option->iaaddr.lifetime_preferred);

        if (lt_valid == 0 || lt_pref > lt_valid) {
                log_dhcp6_client(client, "Valid lifetime of an IA address is zero or preferred lifetime %d > valid lifetime %d",
                                 lt_pref, lt_valid);

                return 0;
        }

        if (be16toh(option->len) + sizeof(DHCP6Option) > sizeof(*addr_option)) {
                r = dhcp6_option_parse_status((DHCP6Option *)addr_option->options, be16toh(option->len) + sizeof(DHCP6Option) - sizeof(*addr_option));
                if (r != 0)
                        return r < 0 ? r: 0;
        }

        addr = new0(DHCP6Address, 1);
        if (!addr)
                return -ENOMEM;

        LIST_INIT(addresses, addr);
        memcpy(&addr->iaaddr, option->data, sizeof(addr->iaaddr));

        LIST_PREPEND(addresses, ia->addresses, addr);

        *lifetime_valid = be32toh(addr->iaaddr.lifetime_valid);

        return 0;
}