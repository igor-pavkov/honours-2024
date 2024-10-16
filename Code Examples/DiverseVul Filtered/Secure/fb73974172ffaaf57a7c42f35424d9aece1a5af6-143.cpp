int selinux_disable(struct selinux_state *state)
{
	if (selinux_initialized(state)) {
		/* Not permitted after initial policy load. */
		return -EINVAL;
	}

	if (selinux_disabled(state)) {
		/* Only do this once. */
		return -EINVAL;
	}

	selinux_mark_disabled(state);

	pr_info("SELinux:  Disabled at runtime.\n");

	/*
	 * Unregister netfilter hooks.
	 * Must be done before security_delete_hooks() to avoid breaking
	 * runtime disable.
	 */
	selinux_nf_ip_exit();

	security_delete_hooks(selinux_hooks, ARRAY_SIZE(selinux_hooks));

	/* Try to destroy the avc node cache */
	avc_disable();

	/* Unregister selinuxfs. */
	exit_sel_fs();

	return 0;
}