static void withdraw_host_rrs(AvahiServer *s) {
    assert(s);

    if (s->hinfo_entry_group)
        avahi_s_entry_group_reset(s->hinfo_entry_group);

    if (s->browse_domain_entry_group)
        avahi_s_entry_group_reset(s->browse_domain_entry_group);

    avahi_interface_monitor_update_rrs(s->monitor, 1);
    s->n_host_rr_pending = 0;
}