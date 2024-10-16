void avahi_server_prepare_response(AvahiServer *s, AvahiInterface *i, AvahiEntry *e, int unicast_response, int auxiliary) {
    assert(s);
    assert(i);
    assert(e);

    avahi_record_list_push(s->record_list, e->record, e->flags & AVAHI_PUBLISH_UNIQUE, unicast_response, auxiliary);
}