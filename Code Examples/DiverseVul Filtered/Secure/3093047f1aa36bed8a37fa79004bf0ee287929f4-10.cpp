static void handle_query_packet(AvahiServer *s, AvahiDnsPacket *p, AvahiInterface *i, const AvahiAddress *a, uint16_t port, int legacy_unicast, int from_local_iface) {
    size_t n;
    int is_probe;

    assert(s);
    assert(p);
    assert(i);
    assert(a);

    assert(avahi_record_list_is_empty(s->record_list));

    is_probe = avahi_dns_packet_get_field(p, AVAHI_DNS_FIELD_NSCOUNT) > 0;

    /* Handle the questions */
    for (n = avahi_dns_packet_get_field(p, AVAHI_DNS_FIELD_QDCOUNT); n > 0; n --) {
        AvahiKey *key;
        int unicast_response = 0;

        if (!(key = avahi_dns_packet_consume_key(p, &unicast_response))) {
            avahi_log_warn(__FILE__": Packet too short or invalid while reading question key. (Maybe a UTF-8 problem?)");
            goto fail;
        }

        if (!legacy_unicast && !from_local_iface) {
            reflect_query(s, i, key);
            if (!unicast_response)
              avahi_cache_start_poof(i->cache, key, a);
        }

        if (avahi_dns_packet_get_field(p, AVAHI_DNS_FIELD_ANCOUNT) == 0 &&
            !(avahi_dns_packet_get_field(p, AVAHI_DNS_FIELD_FLAGS) & AVAHI_DNS_FLAG_TC))
            /* Allow our own queries to be suppressed by incoming
             * queries only when they do not include known answers */
            avahi_query_scheduler_incoming(i->query_scheduler, key);

        avahi_server_prepare_matching_responses(s, i, key, unicast_response);
        avahi_key_unref(key);
    }

    if (!legacy_unicast) {

        /* Known Answer Suppression */
        for (n = avahi_dns_packet_get_field(p, AVAHI_DNS_FIELD_ANCOUNT); n > 0; n --) {
            AvahiRecord *record;
            int unique = 0;

            if (!(record = avahi_dns_packet_consume_record(p, &unique))) {
                avahi_log_warn(__FILE__": Packet too short or invalid while reading known answer record. (Maybe a UTF-8 problem?)");
                goto fail;
            }

            avahi_response_scheduler_suppress(i->response_scheduler, record, a);
            avahi_record_list_drop(s->record_list, record);
            avahi_cache_stop_poof(i->cache, record, a);

            avahi_record_unref(record);
        }

        /* Probe record */
        for (n = avahi_dns_packet_get_field(p, AVAHI_DNS_FIELD_NSCOUNT); n > 0; n --) {
            AvahiRecord *record;
            int unique = 0;

            if (!(record = avahi_dns_packet_consume_record(p, &unique))) {
                avahi_log_warn(__FILE__": Packet too short or invalid while reading probe record. (Maybe a UTF-8 problem?)");
                goto fail;
            }

            if (!avahi_key_is_pattern(record->key)) {
                if (!from_local_iface)
                    reflect_probe(s, i, record);
                incoming_probe(s, record, i);
            }

            avahi_record_unref(record);
        }
    }

    if (!avahi_record_list_is_empty(s->record_list))
        avahi_server_generate_response(s, i, p, a, port, legacy_unicast, is_probe);

    return;

fail:
    avahi_record_list_flush(s->record_list);
}