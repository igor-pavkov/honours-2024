static int selinux_secmark_relabel_packet(u32 sid)
{
	const struct task_security_struct *__tsec;
	u32 tsid;

	__tsec = selinux_cred(current_cred());
	tsid = __tsec->sid;

	return avc_has_perm(&selinux_state,
			    tsid, sid, SECCLASS_PACKET, PACKET__RELABELTO,
			    NULL);
}