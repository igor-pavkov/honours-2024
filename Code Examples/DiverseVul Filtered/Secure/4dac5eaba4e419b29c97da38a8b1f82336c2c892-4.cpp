int dhcp6_option_parse_status(DHCP6Option *option, size_t len) {
        DHCP6StatusOption *statusopt = (DHCP6StatusOption *)option;

        if (len < sizeof(DHCP6StatusOption) ||
            be16toh(option->len) + sizeof(DHCP6Option) < sizeof(DHCP6StatusOption))
                return -ENOBUFS;

        return be16toh(statusopt->status);
}