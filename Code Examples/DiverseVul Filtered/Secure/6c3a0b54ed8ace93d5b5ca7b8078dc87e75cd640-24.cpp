int inplace_cb_query_call(struct module_env* env, struct query_info* qinfo,
	uint16_t flags, struct sockaddr_storage* addr, socklen_t addrlen,
	uint8_t* zone, size_t zonelen, struct module_qstate* qstate,
	struct regional* region)
{
	struct inplace_cb* cb = env->inplace_cb_lists[inplace_cb_query];
	for(; cb; cb=cb->next) {
		fptr_ok(fptr_whitelist_inplace_cb_query(
			(inplace_cb_query_func_type*)cb->cb));
		(void)(*(inplace_cb_query_func_type*)cb->cb)(qinfo, flags,
			qstate, addr, addrlen, zone, zonelen, region,
			cb->id, cb->cb_arg);
	}
	return 1;
}