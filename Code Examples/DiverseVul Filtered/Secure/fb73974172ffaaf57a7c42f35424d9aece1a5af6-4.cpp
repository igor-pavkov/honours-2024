static void cred_init_security(void)
{
	struct cred *cred = (struct cred *) current->real_cred;
	struct task_security_struct *tsec;

	tsec = selinux_cred(cred);
	tsec->osid = tsec->sid = SECINITSID_KERNEL;
}