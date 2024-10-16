static int selinux_key_alloc(struct key *k, const struct cred *cred,
			     unsigned long flags)
{
	const struct task_security_struct *tsec;
	struct key_security_struct *ksec;

	ksec = kzalloc(sizeof(struct key_security_struct), GFP_KERNEL);
	if (!ksec)
		return -ENOMEM;

	tsec = selinux_cred(cred);
	if (tsec->keycreate_sid)
		ksec->sid = tsec->keycreate_sid;
	else
		ksec->sid = tsec->sid;

	k->security = ksec;
	return 0;
}