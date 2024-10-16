static void append_aux_callback(AvahiServer *s, AvahiRecord *r, int flush_cache, void* userdata) {
    int *unicast_response = userdata;

    assert(s);
    assert(r);
    assert(unicast_response);

    avahi_record_list_push(s->record_list, r, flush_cache, *unicast_response, 1);
}