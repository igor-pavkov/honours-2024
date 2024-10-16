static void __exit ebtables_fini(void)
{
	nf_unregister_sockopt(&ebt_sockopts);
	xt_unregister_target(&ebt_standard_target);
	printk(KERN_INFO "Ebtables v2.0 unregistered\n");
}