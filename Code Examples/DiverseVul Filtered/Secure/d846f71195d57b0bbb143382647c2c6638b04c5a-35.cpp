static int __init ebtables_init(void)
{
	int ret;

	ret = xt_register_target(&ebt_standard_target);
	if (ret < 0)
		return ret;
	ret = nf_register_sockopt(&ebt_sockopts);
	if (ret < 0) {
		xt_unregister_target(&ebt_standard_target);
		return ret;
	}

	printk(KERN_INFO "Ebtables v2.0 registered\n");
	return 0;
}