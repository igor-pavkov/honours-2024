ct_print_conn_info(const struct conn *c, const char *log_msg,
                   enum vlog_level vll, bool force, bool rl_on)
{
#define CT_VLOG(RL_ON, LEVEL, ...)                                          \
    do {                                                                    \
        if (RL_ON) {                                                        \
            static struct vlog_rate_limit rl_ = VLOG_RATE_LIMIT_INIT(5, 5); \
            vlog_rate_limit(&this_module, LEVEL, &rl_, __VA_ARGS__);        \
        } else {                                                            \
            vlog(&this_module, LEVEL, __VA_ARGS__);                         \
        }                                                                   \
    } while (0)

    if (OVS_UNLIKELY(force || vlog_is_enabled(&this_module, vll))) {
        if (c->key.dl_type == htons(ETH_TYPE_IP)) {
            CT_VLOG(rl_on, vll, "%s: src ip "IP_FMT" dst ip "IP_FMT" rev src "
                    "ip "IP_FMT" rev dst ip "IP_FMT" src/dst ports "
                    "%"PRIu16"/%"PRIu16" rev src/dst ports "
                    "%"PRIu16"/%"PRIu16" zone/rev zone "
                    "%"PRIu16"/%"PRIu16" nw_proto/rev nw_proto "
                    "%"PRIu8"/%"PRIu8, log_msg,
                    IP_ARGS(c->key.src.addr.ipv4_aligned),
                    IP_ARGS(c->key.dst.addr.ipv4_aligned),
                    IP_ARGS(c->rev_key.src.addr.ipv4_aligned),
                    IP_ARGS(c->rev_key.dst.addr.ipv4_aligned),
                    ntohs(c->key.src.port), ntohs(c->key.dst.port),
                    ntohs(c->rev_key.src.port), ntohs(c->rev_key.dst.port),
                    c->key.zone, c->rev_key.zone, c->key.nw_proto,
                    c->rev_key.nw_proto);
        } else {
            char ip6_s[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &c->key.src.addr.ipv6, ip6_s, sizeof ip6_s);
            char ip6_d[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &c->key.dst.addr.ipv6, ip6_d, sizeof ip6_d);
            char ip6_rs[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &c->rev_key.src.addr.ipv6, ip6_rs,
                      sizeof ip6_rs);
            char ip6_rd[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &c->rev_key.dst.addr.ipv6, ip6_rd,
                      sizeof ip6_rd);

            CT_VLOG(rl_on, vll, "%s: src ip %s dst ip %s rev src ip %s"
                    " rev dst ip %s src/dst ports %"PRIu16"/%"PRIu16
                    " rev src/dst ports %"PRIu16"/%"PRIu16" zone/rev zone "
                    "%"PRIu16"/%"PRIu16" nw_proto/rev nw_proto "
                    "%"PRIu8"/%"PRIu8, log_msg, ip6_s, ip6_d, ip6_rs,
                    ip6_rd, ntohs(c->key.src.port), ntohs(c->key.dst.port),
                    ntohs(c->rev_key.src.port), ntohs(c->rev_key.dst.port),
                    c->key.zone, c->rev_key.zone, c->key.nw_proto,
                    c->rev_key.nw_proto);
        }
    }
}