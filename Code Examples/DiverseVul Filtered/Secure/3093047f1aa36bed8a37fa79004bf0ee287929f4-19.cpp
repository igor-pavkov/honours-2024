static void cleanup_dead(AvahiServer *s) {
    assert(s);

    avahi_cleanup_dead_entries(s);
    avahi_browser_cleanup(s);
}