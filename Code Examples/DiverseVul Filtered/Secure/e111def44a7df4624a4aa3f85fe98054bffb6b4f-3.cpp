static void* reflect_cache_walk_callback(AvahiCache *c, AvahiKey *pattern, AvahiCacheEntry *e, void* userdata) {
    AvahiServer *s = userdata;
    AvahiRecord* r;

    assert(c);
    assert(pattern);
    assert(e);
    assert(s);

    /* Don't reflect cache entry with ipv6 link-local addresses. */
    r = e->record;
    if ((r->key->type == AVAHI_DNS_TYPE_AAAA) &&
            (r->data.aaaa.address.address[0] == 0xFE) &&
            (r->data.aaaa.address.address[1] == 0x80))
      return NULL;

    avahi_record_list_push(s->record_list, e->record, e->cache_flush, 0, 0);
    return NULL;
}