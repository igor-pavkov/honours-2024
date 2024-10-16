void avahi_server_free(AvahiServer* s) {
    assert(s);

    /* Remove all browsers */

    while (s->dns_server_browsers)
        avahi_s_dns_server_browser_free(s->dns_server_browsers);
    while (s->host_name_resolvers)
        avahi_s_host_name_resolver_free(s->host_name_resolvers);
    while (s->address_resolvers)
        avahi_s_address_resolver_free(s->address_resolvers);
    while (s->domain_browsers)
        avahi_s_domain_browser_free(s->domain_browsers);
    while (s->service_type_browsers)
        avahi_s_service_type_browser_free(s->service_type_browsers);
    while (s->service_browsers)
        avahi_s_service_browser_free(s->service_browsers);
    while (s->service_resolvers)
        avahi_s_service_resolver_free(s->service_resolvers);
    while (s->record_browsers)
        avahi_s_record_browser_destroy(s->record_browsers);

    /* Remove all locally rgeistered stuff */

    while(s->entries)
        avahi_entry_free(s, s->entries);

    avahi_interface_monitor_free(s->monitor);

    while (s->groups)
        avahi_entry_group_free(s, s->groups);

    free_slots(s);

    avahi_hashmap_free(s->entries_by_key);
    avahi_record_list_free(s->record_list);
    avahi_hashmap_free(s->record_browser_hashmap);

    if (s->wide_area_lookup_engine)
        avahi_wide_area_engine_free(s->wide_area_lookup_engine);
    avahi_multicast_lookup_engine_free(s->multicast_lookup_engine);

    if (s->cleanup_time_event)
        avahi_time_event_free(s->cleanup_time_event);

    avahi_time_event_queue_free(s->time_event_queue);

    /* Free watches */

    if (s->watch_ipv4)
        s->poll_api->watch_free(s->watch_ipv4);
    if (s->watch_ipv6)
        s->poll_api->watch_free(s->watch_ipv6);

    if (s->watch_legacy_unicast_ipv4)
        s->poll_api->watch_free(s->watch_legacy_unicast_ipv4);
    if (s->watch_legacy_unicast_ipv6)
        s->poll_api->watch_free(s->watch_legacy_unicast_ipv6);

    /* Free sockets */

    if (s->fd_ipv4 >= 0)
        close(s->fd_ipv4);
    if (s->fd_ipv6 >= 0)
        close(s->fd_ipv6);

    if (s->fd_legacy_unicast_ipv4 >= 0)
        close(s->fd_legacy_unicast_ipv4);
    if (s->fd_legacy_unicast_ipv6 >= 0)
        close(s->fd_legacy_unicast_ipv6);

    /* Free other stuff */

    avahi_free(s->host_name);
    avahi_free(s->domain_name);
    avahi_free(s->host_name_fqdn);

    avahi_server_config_free(&s->config);

    avahi_free(s);
}