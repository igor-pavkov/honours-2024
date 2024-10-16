int inplace_cb_query_response_call(struct module_env* env,
	struct module_qstate* qstate, struct dns_msg* response) {
	struct inplace_cb* cb =
		env->inplace_cb_lists[inplace_cb_query_response];
	for(; cb; cb=cb->next) {
		fptr_ok(fptr_whitelist_inplace_cb_query_response(
			(inplace_cb_query_response_func_type*)cb->cb));
		(void)(*(inplace_cb_query_response_func_type*)cb->cb)(qstate,
			response, cb->id, cb->cb_arg);
	}
	return 1;
}