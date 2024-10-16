int inplace_cb_reply_servfail_call(struct module_env* env,
	struct query_info* qinfo, struct module_qstate* qstate,
	struct reply_info* rep, int rcode, struct edns_data* edns,
	struct comm_reply* repinfo, struct regional* region)
{
	/* We are going to servfail. Remove any potential edns options. */
	if(qstate)
		qstate->edns_opts_front_out = NULL;
	return inplace_cb_reply_call_generic(
		env->inplace_cb_lists[inplace_cb_reply_servfail],
		inplace_cb_reply_servfail, qinfo, qstate, rep, rcode, edns, repinfo,
		region);
}