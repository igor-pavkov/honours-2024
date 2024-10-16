static void handle_response_packet(AvahiServer *s, AvahiDnsPacket *p, AvahiInterface *i, const AvahiAddress *a, int from_local_iface) {
    unsigned n;

    assert(s);
    assert(p);
    assert(i);
    assert(a);

    for (n = avahi_dns_packet_get_field(p, AVAHI_DNS_FIELD_ANCOUNT) +
             avahi_dns_packet_get_field(p, AVAHI_DNS_FIELD_ARCOUNT); n > 0; n--) {
        AvahiRecord *record;
        int cache_flush = 0;

        if (!(record = avahi_dns_packet_consume_record(p, &cache_flush))) {
            avahi_log_debug(__FILE__": Packet too short or invalid while reading response record. (Maybe a UTF-8 problem?)");
            break;
        }

        if (!avahi_key_is_pattern(record->key)) {

            if (handle_conflict(s, i, record, cache_flush)) {
                if (!from_local_iface && !avahi_record_is_link_local_address(record))
                    reflect_response(s, i, record, cache_flush);
                avahi_cache_update(i->cache, record, cache_flush, a);
                avahi_response_scheduler_incoming(i->response_scheduler, record, cache_flush);
            }
        }

        avahi_record_unref(record);
    }

    /* If the incoming response contained a conflicting record, some
       records have been scheduled for sending. We need to flush them
       here. */
    if (!avahi_record_list_is_empty(s->record_list))
        avahi_server_generate_response(s, i, NULL, NULL, 0, 0, 1);
}