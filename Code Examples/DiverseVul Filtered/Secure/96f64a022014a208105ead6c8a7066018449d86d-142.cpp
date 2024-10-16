int evdns_resolve_reverse_ipv6(const struct in6_addr *in, int flags, evdns_callback_type callback, void *ptr) {
	return evdns_base_resolve_reverse_ipv6(current_base, in, flags, callback, ptr)
		? 0 : -1;
}