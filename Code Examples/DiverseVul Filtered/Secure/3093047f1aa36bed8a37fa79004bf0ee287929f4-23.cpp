static void register_hinfo(AvahiServer *s) {
    struct utsname utsname;
    AvahiRecord *r;

    assert(s);

    if (!s->config.publish_hinfo)
        return;

    if (s->hinfo_entry_group)
        assert(avahi_s_entry_group_is_empty(s->hinfo_entry_group));
    else
        s->hinfo_entry_group = avahi_s_entry_group_new(s, avahi_host_rr_entry_group_callback, NULL);

    if (!s->hinfo_entry_group) {
        avahi_log_warn("Failed to create HINFO entry group: %s", avahi_strerror(s->error));
        return;
    }

    /* Fill in HINFO rr */
    if ((r = avahi_record_new_full(s->host_name_fqdn, AVAHI_DNS_CLASS_IN, AVAHI_DNS_TYPE_HINFO, AVAHI_DEFAULT_TTL_HOST_NAME))) {

        if (uname(&utsname) < 0)
            avahi_log_warn("uname() failed: %s\n", avahi_strerror(errno));
        else {

            r->data.hinfo.cpu = avahi_strdup(avahi_strup(utsname.machine));
            r->data.hinfo.os = avahi_strdup(avahi_strup(utsname.sysname));

            avahi_log_info("Registering HINFO record with values '%s'/'%s'.", r->data.hinfo.cpu, r->data.hinfo.os);

            if (avahi_server_add(s, s->hinfo_entry_group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, AVAHI_PUBLISH_UNIQUE, r) < 0) {
                avahi_log_warn("Failed to add HINFO RR: %s", avahi_strerror(s->error));
                return;
            }
        }

        avahi_record_unref(r);
    }

    if (avahi_s_entry_group_commit(s->hinfo_entry_group) < 0)
        avahi_log_warn("Failed to commit HINFO entry group: %s", avahi_strerror(s->error));

}