static __init int selinux_init(void)
{
	if (!security_module_enable(&selinux_ops)) {
		selinux_enabled = 0;
		return 0;
	}

	if (!selinux_enabled) {
		printk(KERN_INFO "SELinux:  Disabled at boot.\n");
		return 0;
	}

	printk(KERN_INFO "SELinux:  Initializing.\n");

	/* Set the security state for the initial task. */
	cred_init_security();

	default_noexec = !(VM_DATA_DEFAULT_FLAGS & VM_EXEC);

	sel_inode_cache = kmem_cache_create("selinux_inode_security",
					    sizeof(struct inode_security_struct),
					    0, SLAB_PANIC, NULL);
	avc_init();

	if (register_security(&selinux_ops))
		panic("SELinux: Unable to register with kernel.\n");

	if (avc_add_callback(selinux_netcache_avc_callback, AVC_CALLBACK_RESET))
		panic("SELinux: Unable to register AVC netcache callback\n");

	if (selinux_enforcing)
		printk(KERN_DEBUG "SELinux:  Starting in enforcing mode\n");
	else
		printk(KERN_DEBUG "SELinux:  Starting in permissive mode\n");

	return 0;
}