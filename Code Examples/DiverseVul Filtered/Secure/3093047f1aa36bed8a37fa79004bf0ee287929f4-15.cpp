static void append_aux_records_to_list(AvahiServer *s, AvahiInterface *i, AvahiRecord *r, int unicast_response) {
    assert(s);
    assert(r);

    avahi_server_enumerate_aux_records(s, i, r, append_aux_callback, &unicast_response);
}