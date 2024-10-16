int evdns_resolve_reverse(const struct in_addr *in, int flags, evdns_callback_type callback, void *ptr) {
	return evdns_base_resolve_reverse(current_base, in, flags, callback, ptr)
		? 0 : -1;
}