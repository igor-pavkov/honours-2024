int inplace_cb_reply_local_call(struct module_env* env,
	struct query_info* qinfo, struct module_qstate* qstate,
	struct reply_info* rep, int rcode, struct edns_data* edns,
	struct comm_reply* repinfo, struct regional* region)
{
	return inplace_cb_reply_call_generic(
		env->inplace_cb_lists[inplace_cb_reply_local], inplace_cb_reply_local,
		qinfo, qstate, rep, rcode, edns, repinfo, region);
}