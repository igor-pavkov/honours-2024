static int call_netdevice_notifiers_info(unsigned long val,
					 struct netdev_notifier_info *info)
{
	ASSERT_RTNL();
	return raw_notifier_call_chain(&netdev_chain, val, info);
}