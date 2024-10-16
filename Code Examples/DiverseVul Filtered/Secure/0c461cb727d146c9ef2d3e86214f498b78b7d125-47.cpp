static void selinux_nf_ip_exit(void)
{
	printk(KERN_DEBUG "SELinux:  Unregistering netfilter hooks\n");

	nf_unregister_hooks(selinux_nf_ops, ARRAY_SIZE(selinux_nf_ops));
}