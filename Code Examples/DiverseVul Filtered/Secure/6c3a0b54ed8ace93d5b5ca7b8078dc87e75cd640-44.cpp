static int inplace_cb_reply_call_generic(
    struct inplace_cb* callback_list, enum inplace_cb_list_type type,
	struct query_info* qinfo, struct module_qstate* qstate,
	struct reply_info* rep, int rcode, struct edns_data* edns,
	struct comm_reply* repinfo, struct regional* region)
{
	struct inplace_cb* cb;
	struct edns_option* opt_list_out = NULL;
#if defined(EXPORT_ALL_SYMBOLS)
	(void)type; /* param not used when fptr_ok disabled */
#endif
	if(qstate)
		opt_list_out = qstate->edns_opts_front_out;
	for(cb=callback_list; cb; cb=cb->next) {
		fptr_ok(fptr_whitelist_inplace_cb_reply_generic(
			(inplace_cb_reply_func_type*)cb->cb, type));
		(void)(*(inplace_cb_reply_func_type*)cb->cb)(qinfo, qstate, rep,
			rcode, edns, &opt_list_out, repinfo, region, cb->id, cb->cb_arg);
	}
	edns->opt_list = opt_list_out;
	return 1;
}