AvahiServer *avahi_server_new(const AvahiPoll *poll_api, const AvahiServerConfig *sc, AvahiServerCallback callback, void* userdata, int *error) {
    AvahiServer *s;
    int e;

    if (sc && (e = valid_server_config(sc)) < 0) {
        if (error)
            *error = e;
        return NULL;
    }

    if (!(s = avahi_new(AvahiServer, 1))) {
        if (error)
            *error = AVAHI_ERR_NO_MEMORY;

        return NULL;
    }

    s->poll_api = poll_api;

    if (sc)
        avahi_server_config_copy(&s->config, sc);
    else
        avahi_server_config_init(&s->config);

    if ((e = setup_sockets(s)) < 0) {
        if (error)
            *error = e;

        avahi_server_config_free(&s->config);
        avahi_free(s);

        return NULL;
    }

    s->n_host_rr_pending = 0;
    s->need_entry_cleanup = 0;
    s->need_group_cleanup = 0;
    s->need_browser_cleanup = 0;
    s->cleanup_time_event = NULL;
    s->hinfo_entry_group = NULL;
    s->browse_domain_entry_group = NULL;
    s->error = AVAHI_OK;
    s->state = AVAHI_SERVER_INVALID;

    s->callback = callback;
    s->userdata = userdata;

    s->time_event_queue = avahi_time_event_queue_new(poll_api);

    s->entries_by_key = avahi_hashmap_new((AvahiHashFunc) avahi_key_hash, (AvahiEqualFunc) avahi_key_equal, NULL, NULL);
    AVAHI_LLIST_HEAD_INIT(AvahiEntry, s->entries);
    AVAHI_LLIST_HEAD_INIT(AvahiGroup, s->groups);

    s->record_browser_hashmap = avahi_hashmap_new((AvahiHashFunc) avahi_key_hash, (AvahiEqualFunc) avahi_key_equal, NULL, NULL);
    AVAHI_LLIST_HEAD_INIT(AvahiSRecordBrowser, s->record_browsers);
    AVAHI_LLIST_HEAD_INIT(AvahiSHostNameResolver, s->host_name_resolvers);
    AVAHI_LLIST_HEAD_INIT(AvahiSAddressResolver, s->address_resolvers);
    AVAHI_LLIST_HEAD_INIT(AvahiSDomainBrowser, s->domain_browsers);
    AVAHI_LLIST_HEAD_INIT(AvahiSServiceTypeBrowser, s->service_type_browsers);
    AVAHI_LLIST_HEAD_INIT(AvahiSServiceBrowser, s->service_browsers);
    AVAHI_LLIST_HEAD_INIT(AvahiSServiceResolver, s->service_resolvers);
    AVAHI_LLIST_HEAD_INIT(AvahiSDNSServerBrowser, s->dns_server_browsers);

    s->legacy_unicast_reflect_slots = NULL;
    s->legacy_unicast_reflect_id = 0;

    s->record_list = avahi_record_list_new();

    /* Get host name */
    s->host_name = s->config.host_name ? avahi_normalize_name_strdup(s->config.host_name) : avahi_get_host_name_strdup();
    s->host_name[strcspn(s->host_name, ".")] = 0;
    s->domain_name = s->config.domain_name ? avahi_normalize_name_strdup(s->config.domain_name) : avahi_strdup("local");
    s->host_name_fqdn = NULL;
    update_fqdn(s);

    do {
        s->local_service_cookie = (uint32_t) rand() * (uint32_t) rand();
    } while (s->local_service_cookie == AVAHI_SERVICE_COOKIE_INVALID);

    if (s->config.enable_wide_area) {
        s->wide_area_lookup_engine = avahi_wide_area_engine_new(s);
        avahi_wide_area_set_servers(s->wide_area_lookup_engine, s->config.wide_area_servers, s->config.n_wide_area_servers);
    } else
        s->wide_area_lookup_engine = NULL;

    s->multicast_lookup_engine = avahi_multicast_lookup_engine_new(s);

    s->monitor = avahi_interface_monitor_new(s);
    avahi_interface_monitor_sync(s->monitor);

    register_localhost(s);
    register_stuff(s);

    return s;
}