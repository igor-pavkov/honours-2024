static int __init tun_init(void)
{
	int ret = 0;

	printk(KERN_INFO "tun: %s, %s\n", DRV_DESCRIPTION, DRV_VERSION);
	printk(KERN_INFO "tun: %s\n", DRV_COPYRIGHT);

	ret = rtnl_link_register(&tun_link_ops);
	if (ret) {
		printk(KERN_ERR "tun: Can't register link_ops\n");
		goto err_linkops;
	}

	ret = misc_register(&tun_miscdev);
	if (ret) {
		printk(KERN_ERR "tun: Can't register misc device %d\n", TUN_MINOR);
		goto err_misc;
	}
	return  0;
err_misc:
	rtnl_link_unregister(&tun_link_ops);
err_linkops:
	return ret;
}