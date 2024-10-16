void avahi_server_generate_response(AvahiServer *s, AvahiInterface *i, AvahiDnsPacket *p, const AvahiAddress *a, uint16_t port, int legacy_unicast, int immediately) {

    assert(s);
    assert(i);
    assert(!legacy_unicast || (a && port > 0 && p));

    if (legacy_unicast) {
        AvahiDnsPacket *reply;
        AvahiRecord *r;

        if (!(reply = avahi_dns_packet_new_reply(p, 512 + AVAHI_DNS_PACKET_EXTRA_SIZE /* unicast DNS maximum packet size is 512 */ , 1, 1)))
            return; /* OOM */

        while ((r = avahi_record_list_next(s->record_list, NULL, NULL, NULL))) {

            append_aux_records_to_list(s, i, r, 0);

            if (avahi_dns_packet_append_record(reply, r, 0, 10))
                avahi_dns_packet_inc_field(reply, AVAHI_DNS_FIELD_ANCOUNT);
            else {
                char *t = avahi_record_to_string(r);
                avahi_log_warn("Record [%s] not fitting in legacy unicast packet, dropping.", t);
                avahi_free(t);
            }

            avahi_record_unref(r);
        }

        if (avahi_dns_packet_get_field(reply, AVAHI_DNS_FIELD_ANCOUNT) != 0)
            avahi_interface_send_packet_unicast(i, reply, a, port);

        avahi_dns_packet_free(reply);

    } else {
        int unicast_response, flush_cache, auxiliary;
        AvahiDnsPacket *reply = NULL;
        AvahiRecord *r;

        /* In case the query packet was truncated never respond
        immediately, because known answer suppression records might be
        contained in later packets */
        int tc = p && !!(avahi_dns_packet_get_field(p, AVAHI_DNS_FIELD_FLAGS) & AVAHI_DNS_FLAG_TC);

        while ((r = avahi_record_list_next(s->record_list, &flush_cache, &unicast_response, &auxiliary))) {

            int im = immediately;

            /* Only send the response immediately if it contains a
             * unique entry AND it is not in reply to a truncated
             * packet AND it is not an auxiliary record AND all other
             * responses for this record are unique too. */

            if (flush_cache && !tc && !auxiliary && avahi_record_list_all_flush_cache(s->record_list))
                im = 1;

            if (!avahi_interface_post_response(i, r, flush_cache, a, im) && unicast_response) {

                /* Due to some reasons the record has not been scheduled.
                 * The client requested an unicast response in that
                 * case. Therefore we prepare such a response */

                append_aux_records_to_list(s, i, r, unicast_response);

                for (;;) {

                    if (!reply) {
                        assert(p);

                        if (!(reply = avahi_dns_packet_new_reply(p, i->hardware->mtu, 0, 0)))
                            break; /* OOM */
                    }

                    if (avahi_dns_packet_append_record(reply, r, flush_cache, 0)) {

                        /* Appending this record succeeded, so incremeant
                         * the specific header field, and return to the caller */

                        avahi_dns_packet_inc_field(reply, AVAHI_DNS_FIELD_ANCOUNT);
                        break;
                    }

                    if (avahi_dns_packet_get_field(reply, AVAHI_DNS_FIELD_ANCOUNT) == 0) {
                        size_t size;

                        /* The record is too large for one packet, so create a larger packet */

                        avahi_dns_packet_free(reply);
                        size = avahi_record_get_estimate_size(r) + AVAHI_DNS_PACKET_HEADER_SIZE;

                        if (!(reply = avahi_dns_packet_new_reply(p, size + AVAHI_DNS_PACKET_EXTRA_SIZE, 0, 1)))
                            break; /* OOM */

                        if (avahi_dns_packet_append_record(reply, r, flush_cache, 0)) {

                            /* Appending this record succeeded, so incremeant
                             * the specific header field, and return to the caller */

                            avahi_dns_packet_inc_field(reply, AVAHI_DNS_FIELD_ANCOUNT);
                            break;

                        }  else {

                            /* We completely fucked up, there's
                             * nothing we can do. The RR just doesn't
                             * fit in. Let's ignore it. */

                            char *t;
                            avahi_dns_packet_free(reply);
                            reply = NULL;
                            t = avahi_record_to_string(r);
                            avahi_log_warn("Record [%s] too large, doesn't fit in any packet!", t);
                            avahi_free(t);
                            break;
                        }
                    }

                    /* Appending the record didn't succeeed, so let's send this packet, and create a new one */
                    avahi_interface_send_packet_unicast(i, reply, a, port);
                    avahi_dns_packet_free(reply);
                    reply = NULL;
                }
            }

            avahi_record_unref(r);
        }

        if (reply) {
            if (avahi_dns_packet_get_field(reply, AVAHI_DNS_FIELD_ANCOUNT) != 0)
                avahi_interface_send_packet_unicast(i, reply, a, port);
            avahi_dns_packet_free(reply);
        }
    }

    avahi_record_list_flush(s->record_list);
}