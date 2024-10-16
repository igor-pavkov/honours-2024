static void tun_cleanup(void)
{
	misc_deregister(&tun_miscdev);
	rtnl_link_unregister(&tun_link_ops);
}