static void dispatch_legacy_unicast_packet(AvahiServer *s, AvahiDnsPacket *p) {
    AvahiInterface *j;
    AvahiLegacyUnicastReflectSlot *slot;

    assert(s);
    assert(p);

    if (avahi_dns_packet_check_valid(p) < 0 || avahi_dns_packet_is_query(p)) {
        avahi_log_warn("Received invalid packet.");
        return;
    }

    if (!(slot = find_slot(s, avahi_dns_packet_get_field(p, AVAHI_DNS_FIELD_ID)))) {
        avahi_log_warn("Received legacy unicast response with unknown id");
        return;
    }

    if (!(j = avahi_interface_monitor_get_interface(s->monitor, slot->interface, slot->address.proto)) ||
        !j->announcing)
        return;

    /* Patch the original ID into this response */
    avahi_dns_packet_set_field(p, AVAHI_DNS_FIELD_ID, slot->original_id);

    /* Forward the response to the correct client */
    avahi_interface_send_packet_unicast(j, p, &slot->address, slot->port);

    /* Undo changes to packet */
    avahi_dns_packet_set_field(p, AVAHI_DNS_FIELD_ID, slot->id);
}