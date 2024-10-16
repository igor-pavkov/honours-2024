int inplace_cb_edns_back_parsed_call(struct module_env* env, 
	struct module_qstate* qstate)
{
	struct inplace_cb* cb =
		env->inplace_cb_lists[inplace_cb_edns_back_parsed];
	for(; cb; cb=cb->next) {
		fptr_ok(fptr_whitelist_inplace_cb_edns_back_parsed(
			(inplace_cb_edns_back_parsed_func_type*)cb->cb));
		(void)(*(inplace_cb_edns_back_parsed_func_type*)cb->cb)(qstate,
			cb->id, cb->cb_arg);
	}
	return 1;
}