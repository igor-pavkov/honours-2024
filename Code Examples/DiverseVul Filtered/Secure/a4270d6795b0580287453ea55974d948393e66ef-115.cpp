int dev_alloc_name(struct net_device *dev, const char *name)
{
	return dev_alloc_name_ns(dev_net(dev), dev, name);
}