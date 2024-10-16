static int is_mdns_mcast_address(const AvahiAddress *a) {
    AvahiAddress b;
    assert(a);

    avahi_address_parse(a->proto == AVAHI_PROTO_INET ? AVAHI_IPV4_MCAST_GROUP : AVAHI_IPV6_MCAST_GROUP, a->proto, &b);
    return avahi_address_cmp(a, &b) == 0;
}