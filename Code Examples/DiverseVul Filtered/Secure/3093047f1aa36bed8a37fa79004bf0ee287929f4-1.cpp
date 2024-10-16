static void register_browse_domain(AvahiServer *s) {
    assert(s);

    if (!s->config.publish_domain)
        return;

    if (avahi_domain_equal(s->domain_name, "local"))
        return;

    if (s->browse_domain_entry_group)
        assert(avahi_s_entry_group_is_empty(s->browse_domain_entry_group));
    else
        s->browse_domain_entry_group = avahi_s_entry_group_new(s, NULL, NULL);

    if (!s->browse_domain_entry_group) {
        avahi_log_warn("Failed to create browse domain entry group: %s", avahi_strerror(s->error));
        return;
    }

    if (avahi_server_add_ptr(s, s->browse_domain_entry_group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 0, AVAHI_DEFAULT_TTL, "b._dns-sd._udp.local", s->domain_name) < 0) {
        avahi_log_warn("Failed to add browse domain RR: %s", avahi_strerror(s->error));
        return;
    }

    if (avahi_s_entry_group_commit(s->browse_domain_entry_group) < 0)
        avahi_log_warn("Failed to commit browse domain entry group: %s", avahi_strerror(s->error));
}