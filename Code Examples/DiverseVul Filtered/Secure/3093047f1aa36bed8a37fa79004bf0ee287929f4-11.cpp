void avahi_server_enumerate_aux_records(AvahiServer *s, AvahiInterface *i, AvahiRecord *r, void (*callback)(AvahiServer *s, AvahiRecord *r, int flush_cache, void* userdata), void* userdata) {
    assert(s);
    assert(i);
    assert(r);
    assert(callback);

    /* Call the specified callback far all records referenced by the one specified in *r */

    if (r->key->clazz == AVAHI_DNS_CLASS_IN) {
        if (r->key->type == AVAHI_DNS_TYPE_PTR) {
            enum_aux_records(s, i, r->data.ptr.name, AVAHI_DNS_TYPE_SRV, callback, userdata);
            enum_aux_records(s, i, r->data.ptr.name, AVAHI_DNS_TYPE_TXT, callback, userdata);
        } else if (r->key->type == AVAHI_DNS_TYPE_SRV) {
            enum_aux_records(s, i, r->data.srv.name, AVAHI_DNS_TYPE_A, callback, userdata);
            enum_aux_records(s, i, r->data.srv.name, AVAHI_DNS_TYPE_AAAA, callback, userdata);
        } else if (r->key->type == AVAHI_DNS_TYPE_CNAME)
            enum_aux_records(s, i, r->data.cname.name, AVAHI_DNS_TYPE_ANY, callback, userdata);
    }
}