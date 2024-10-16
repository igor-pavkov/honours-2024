static int selinux_vm_enough_memory(struct mm_struct *mm, long pages)
{
	int rc, cap_sys_admin = 0;

	rc = cred_has_capability(current_cred(), CAP_SYS_ADMIN,
				 SECURITY_CAP_NOAUDIT, true);
	if (rc == 0)
		cap_sys_admin = 1;

	return cap_sys_admin;
}