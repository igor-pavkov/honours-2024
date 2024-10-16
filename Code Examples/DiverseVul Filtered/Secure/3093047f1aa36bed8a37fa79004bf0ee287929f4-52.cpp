static void register_localhost(AvahiServer *s) {
    AvahiAddress a;
    assert(s);

    /* Add localhost entries */
    avahi_address_parse("127.0.0.1", AVAHI_PROTO_INET, &a);
    avahi_server_add_address(s, NULL, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, AVAHI_PUBLISH_NO_PROBE|AVAHI_PUBLISH_NO_ANNOUNCE, "localhost", &a);

    avahi_address_parse("::1", AVAHI_PROTO_INET6, &a);
    avahi_server_add_address(s, NULL, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, AVAHI_PUBLISH_NO_PROBE|AVAHI_PUBLISH_NO_ANNOUNCE, "ip6-localhost", &a);
}