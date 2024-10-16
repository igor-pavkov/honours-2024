static void selinux_cred_transfer(struct cred *new, const struct cred *old)
{
	const struct task_security_struct *old_tsec = selinux_cred(old);
	struct task_security_struct *tsec = selinux_cred(new);

	*tsec = *old_tsec;
}