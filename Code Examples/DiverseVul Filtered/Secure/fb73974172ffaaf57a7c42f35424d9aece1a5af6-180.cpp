static int selinux_kernel_act_as(struct cred *new, u32 secid)
{
	struct task_security_struct *tsec = selinux_cred(new);
	u32 sid = current_sid();
	int ret;

	ret = avc_has_perm(&selinux_state,
			   sid, secid,
			   SECCLASS_KERNEL_SERVICE,
			   KERNEL_SERVICE__USE_AS_OVERRIDE,
			   NULL);
	if (ret == 0) {
		tsec->sid = secid;
		tsec->create_sid = 0;
		tsec->keycreate_sid = 0;
		tsec->sockcreate_sid = 0;
	}
	return ret;
}