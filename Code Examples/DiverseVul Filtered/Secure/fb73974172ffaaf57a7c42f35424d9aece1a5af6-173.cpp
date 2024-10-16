static inline u32 cred_sid(const struct cred *cred)
{
	const struct task_security_struct *tsec;

	tsec = selinux_cred(cred);
	return tsec->sid;
}