static void* reflect_cache_walk_callback(AvahiCache *c, AvahiKey *pattern, AvahiCacheEntry *e, void* userdata) {
    AvahiServer *s = userdata;

    assert(c);
    assert(pattern);
    assert(e);
    assert(s);

    avahi_record_list_push(s->record_list, e->record, e->cache_flush, 0, 0);
    return NULL;
}