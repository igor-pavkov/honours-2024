static int selinux_lockdown(enum lockdown_reason what)
{
	struct common_audit_data ad;
	u32 sid = current_sid();
	int invalid_reason = (what <= LOCKDOWN_NONE) ||
			     (what == LOCKDOWN_INTEGRITY_MAX) ||
			     (what >= LOCKDOWN_CONFIDENTIALITY_MAX);

	if (WARN(invalid_reason, "Invalid lockdown reason")) {
		audit_log(audit_context(),
			  GFP_ATOMIC, AUDIT_SELINUX_ERR,
			  "lockdown_reason=invalid");
		return -EINVAL;
	}

	ad.type = LSM_AUDIT_DATA_LOCKDOWN;
	ad.u.reason = what;

	if (what <= LOCKDOWN_INTEGRITY_MAX)
		return avc_has_perm(&selinux_state,
				    sid, sid, SECCLASS_LOCKDOWN,
				    LOCKDOWN__INTEGRITY, &ad);
	else
		return avc_has_perm(&selinux_state,
				    sid, sid, SECCLASS_LOCKDOWN,
				    LOCKDOWN__CONFIDENTIALITY, &ad);
}