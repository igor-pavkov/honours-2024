void avahi_server_config_free(AvahiServerConfig *c) {
    assert(c);

    avahi_free(c->host_name);
    avahi_free(c->domain_name);
    avahi_string_list_free(c->browse_domains);
    avahi_string_list_free(c->allow_interfaces);
    avahi_string_list_free(c->deny_interfaces);
}