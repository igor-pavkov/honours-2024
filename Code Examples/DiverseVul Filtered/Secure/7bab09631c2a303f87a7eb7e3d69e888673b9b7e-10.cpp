void xfrm_audit_policy_add(struct xfrm_policy *xp, int result, bool task_valid)
{
	struct audit_buffer *audit_buf;

	audit_buf = xfrm_audit_start("SPD-add");
	if (audit_buf == NULL)
		return;
	xfrm_audit_helper_usrinfo(task_valid, audit_buf);
	audit_log_format(audit_buf, " res=%u", result);
	xfrm_audit_common_policyinfo(xp, audit_buf);
	audit_log_end(audit_buf);
}