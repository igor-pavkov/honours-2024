void avahi_server_set_data(AvahiServer *s, void* userdata) {
    assert(s);

    s->userdata = userdata;
}