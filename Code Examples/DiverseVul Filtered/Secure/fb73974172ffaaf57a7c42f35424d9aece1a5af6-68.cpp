static int selinux_socket_create(int family, int type,
				 int protocol, int kern)
{
	const struct task_security_struct *tsec = selinux_cred(current_cred());
	u32 newsid;
	u16 secclass;
	int rc;

	if (kern)
		return 0;

	secclass = socket_type_to_security_class(family, type, protocol);
	rc = socket_sockcreate_sid(tsec, secclass, &newsid);
	if (rc)
		return rc;

	return avc_has_perm(&selinux_state,
			    tsec->sid, newsid, secclass, SOCKET__CREATE, NULL);
}