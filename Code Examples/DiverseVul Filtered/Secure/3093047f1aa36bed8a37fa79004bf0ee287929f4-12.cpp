void avahi_host_rr_entry_group_callback(AvahiServer *s, AvahiSEntryGroup *g, AvahiEntryGroupState state, AVAHI_GCC_UNUSED void *userdata) {
    assert(s);
    assert(g);

    if (state == AVAHI_ENTRY_GROUP_REGISTERING &&
        s->state == AVAHI_SERVER_REGISTERING)
        s->n_host_rr_pending ++;

    else if (state == AVAHI_ENTRY_GROUP_COLLISION &&
        (s->state == AVAHI_SERVER_REGISTERING || s->state == AVAHI_SERVER_RUNNING)) {
        withdraw_host_rrs(s);
        server_set_state(s, AVAHI_SERVER_COLLISION);

    } else if (state == AVAHI_ENTRY_GROUP_ESTABLISHED &&
               s->state == AVAHI_SERVER_REGISTERING)
        avahi_server_decrease_host_rr_pending(s);
}