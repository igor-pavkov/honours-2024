int evdns_resolve_ipv6(const char *name, int flags,
    evdns_callback_type callback, void *ptr) {
	return evdns_base_resolve_ipv6(current_base, name, flags, callback, ptr)
		? 0 : -1;
}