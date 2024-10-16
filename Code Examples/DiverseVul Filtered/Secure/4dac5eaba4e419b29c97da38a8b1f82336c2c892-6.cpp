int dhcp6_option_parse_ip6addrs(uint8_t *optval, uint16_t optlen,
                                struct in6_addr **addrs, size_t count,
                                size_t *allocated) {

        if (optlen == 0 || optlen % sizeof(struct in6_addr) != 0)
                return -EINVAL;

        if (!GREEDY_REALLOC(*addrs, *allocated,
                            count * sizeof(struct in6_addr) + optlen))
                return -ENOMEM;

        memcpy(*addrs + count, optval, optlen);

        count += optlen / sizeof(struct in6_addr);

        return count;
}