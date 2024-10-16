static void selinux_nf_ip_exit(void)
{
	pr_debug("SELinux:  Unregistering netfilter hooks\n");

	unregister_pernet_subsys(&selinux_net_ops);
}