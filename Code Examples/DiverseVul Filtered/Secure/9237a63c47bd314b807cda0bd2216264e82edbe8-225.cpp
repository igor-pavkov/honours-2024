str_to_nat_range(const char *s, struct ofpact_nat *on)
{
    char ipv6_s[IPV6_SCAN_LEN + 1];
    int n = 0;

    on->range_af = AF_UNSPEC;
    if (ovs_scan_len(s, &n, IP_SCAN_FMT,
                     IP_SCAN_ARGS(&on->range.addr.ipv4.min))) {
        on->range_af = AF_INET;

        if (s[n] == '-') {
            n++;
            if (!ovs_scan_len(s, &n, IP_SCAN_FMT,
                              IP_SCAN_ARGS(&on->range.addr.ipv4.max))
                || (ntohl(on->range.addr.ipv4.max)
                    < ntohl(on->range.addr.ipv4.min))) {
                goto error;
            }
        }
    } else if ((ovs_scan_len(s, &n, IPV6_SCAN_FMT, ipv6_s)
                || ovs_scan_len(s, &n, "["IPV6_SCAN_FMT"]", ipv6_s))
               && inet_pton(AF_INET6, ipv6_s, &on->range.addr.ipv6.min) == 1) {
        on->range_af = AF_INET6;

        if (s[n] == '-') {
            n++;
            if (!(ovs_scan_len(s, &n, IPV6_SCAN_FMT, ipv6_s)
                  || ovs_scan_len(s, &n, "["IPV6_SCAN_FMT"]", ipv6_s))
                || inet_pton(AF_INET6, ipv6_s, &on->range.addr.ipv6.max) != 1
                || memcmp(&on->range.addr.ipv6.max, &on->range.addr.ipv6.min,
                          sizeof on->range.addr.ipv6.max) < 0) {
                goto error;
            }
        }
    }
    if (on->range_af != AF_UNSPEC && s[n] == ':') {
        n++;
        if (!ovs_scan_len(s, &n, "%"SCNu16, &on->range.proto.min)) {
            goto error;
        }
        if (s[n] == '-') {
            n++;
            if (!ovs_scan_len(s, &n, "%"SCNu16, &on->range.proto.max)
                || on->range.proto.max < on->range.proto.min) {
                goto error;
            }
        }
    }
    if (strlen(s) != n) {
        return xasprintf("garbage (%s) after nat range \"%s\" (pos: %d)",
                         &s[n], s, n);
    }
    return NULL;
error:
    return xasprintf("invalid nat range \"%s\"", s);
}