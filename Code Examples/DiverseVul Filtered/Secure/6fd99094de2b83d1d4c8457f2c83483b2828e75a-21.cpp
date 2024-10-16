int __init ndisc_late_init(void)
{
	return register_netdevice_notifier(&ndisc_netdev_notifier);
}