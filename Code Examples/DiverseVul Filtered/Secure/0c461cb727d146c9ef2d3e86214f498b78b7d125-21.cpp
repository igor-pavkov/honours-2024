int selinux_disable(void)
{
	if (ss_initialized) {
		/* Not permitted after initial policy load. */
		return -EINVAL;
	}

	if (selinux_disabled) {
		/* Only do this once. */
		return -EINVAL;
	}

	printk(KERN_INFO "SELinux:  Disabled at runtime.\n");

	selinux_disabled = 1;
	selinux_enabled = 0;

	security_delete_hooks(selinux_hooks, ARRAY_SIZE(selinux_hooks));

	/* Try to destroy the avc node cache */
	avc_disable();

	/* Unregister netfilter hooks. */
	selinux_nf_ip_exit();

	/* Unregister selinuxfs. */
	exit_sel_fs();

	return 0;
}