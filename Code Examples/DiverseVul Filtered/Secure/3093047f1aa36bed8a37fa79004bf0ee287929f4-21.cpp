void avahi_server_decrease_host_rr_pending(AvahiServer *s) {
    assert(s);

    assert(s->n_host_rr_pending > 0);

    if (--s->n_host_rr_pending == 0)
        server_set_state(s, AVAHI_SERVER_RUNNING);
}