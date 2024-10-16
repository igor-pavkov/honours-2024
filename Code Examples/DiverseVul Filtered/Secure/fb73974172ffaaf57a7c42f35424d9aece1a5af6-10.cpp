void selinux_complete_init(void)
{
	pr_debug("SELinux:  Completing initialization.\n");

	/* Set up any superblocks initialized prior to the policy load. */
	pr_debug("SELinux:  Setting up existing superblocks.\n");
	iterate_supers(delayed_superblock_init, NULL);
}