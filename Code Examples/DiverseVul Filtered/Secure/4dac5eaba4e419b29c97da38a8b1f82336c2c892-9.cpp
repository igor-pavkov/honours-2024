static int dhcp6_option_parse_pdprefix(DHCP6Option *option, DHCP6IA *ia,
                                       uint32_t *lifetime_valid) {
        DHCP6PDPrefixOption *pdprefix_option = (DHCP6PDPrefixOption *)option;
        DHCP6Address *prefix;
        uint32_t lt_valid, lt_pref;
        int r;

        if (be16toh(option->len) + sizeof(DHCP6Option) < sizeof(*pdprefix_option))
                return -ENOBUFS;

        lt_valid = be32toh(pdprefix_option->iapdprefix.lifetime_valid);
        lt_pref = be32toh(pdprefix_option->iapdprefix.lifetime_preferred);

        if (lt_valid == 0 || lt_pref > lt_valid) {
                log_dhcp6_client(client, "Valid lifetieme of a PD prefix is zero or preferred lifetime %d > valid lifetime %d",
                                 lt_pref, lt_valid);

                return 0;
        }

        if (be16toh(option->len) + sizeof(DHCP6Option) > sizeof(*pdprefix_option)) {
                r = dhcp6_option_parse_status((DHCP6Option *)pdprefix_option->options, be16toh(option->len) + sizeof(DHCP6Option) - sizeof(*pdprefix_option));
                if (r != 0)
                        return r < 0 ? r: 0;
        }

        prefix = new0(DHCP6Address, 1);
        if (!prefix)
                return -ENOMEM;

        LIST_INIT(addresses, prefix);
        memcpy(&prefix->iapdprefix, option->data, sizeof(prefix->iapdprefix));

        LIST_PREPEND(addresses, ia->addresses, prefix);

        *lifetime_valid = be32toh(prefix->iapdprefix.lifetime_valid);

        return 0;
}