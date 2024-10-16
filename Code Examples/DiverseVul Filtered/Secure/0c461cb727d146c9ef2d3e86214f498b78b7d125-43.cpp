static int __init selinux_nf_ip_init(void)
{
	int err;

	if (!selinux_enabled)
		return 0;

	printk(KERN_DEBUG "SELinux:  Registering netfilter hooks\n");

	err = nf_register_hooks(selinux_nf_ops, ARRAY_SIZE(selinux_nf_ops));
	if (err)
		panic("SELinux: nf_register_hooks: error %d\n", err);

	return 0;
}