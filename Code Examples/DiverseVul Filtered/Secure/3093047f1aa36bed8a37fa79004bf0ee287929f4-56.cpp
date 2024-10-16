static void reflect_legacy_unicast_query_packet(AvahiServer *s, AvahiDnsPacket *p, AvahiInterface *i, const AvahiAddress *a, uint16_t port) {
    AvahiLegacyUnicastReflectSlot *slot;
    AvahiInterface *j;

    assert(s);
    assert(p);
    assert(i);
    assert(a);
    assert(port > 0);
    assert(i->protocol == a->proto);

    if (!s->config.enable_reflector)
        return;

    /* Reflecting legacy unicast queries is a little more complicated
       than reflecting normal queries, since we must route the
       responses back to the right client. Therefore we must store
       some information for finding the right client contact data for
       response packets. In contrast to normal queries legacy
       unicast query and response packets are reflected untouched and
       are not reassembled into larger packets */

    if (!(slot = allocate_slot(s))) {
        /* No slot available, we drop this legacy unicast query */
        avahi_log_warn("No slot available for legacy unicast reflection, dropping query packet.");
        return;
    }

    slot->original_id = avahi_dns_packet_get_field(p, AVAHI_DNS_FIELD_ID);
    slot->address = *a;
    slot->port = port;
    slot->interface = i->hardware->index;

    avahi_elapse_time(&slot->elapse_time, 2000, 0);
    slot->time_event = avahi_time_event_new(s->time_event_queue, &slot->elapse_time, legacy_unicast_reflect_slot_timeout, slot);

    /* Patch the packet with our new locally generatet id */
    avahi_dns_packet_set_field(p, AVAHI_DNS_FIELD_ID, slot->id);

    for (j = s->monitor->interfaces; j; j = j->interface_next)
        if (j->announcing &&
            j != i &&
            (s->config.reflect_ipv || j->protocol == i->protocol)) {

            if (j->protocol == AVAHI_PROTO_INET && s->fd_legacy_unicast_ipv4 >= 0) {
                avahi_send_dns_packet_ipv4(s->fd_legacy_unicast_ipv4, j->hardware->index, p, NULL, NULL, 0);
            } else if (j->protocol == AVAHI_PROTO_INET6 && s->fd_legacy_unicast_ipv6 >= 0)
                avahi_send_dns_packet_ipv6(s->fd_legacy_unicast_ipv6, j->hardware->index, p, NULL, NULL, 0);
        }

    /* Reset the id */
    avahi_dns_packet_set_field(p, AVAHI_DNS_FIELD_ID, slot->original_id);
}