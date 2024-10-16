int call_netdevice_notifiers(unsigned long val, struct net_device *dev)
{
	return call_netdevice_notifiers_extack(val, dev, NULL);
}