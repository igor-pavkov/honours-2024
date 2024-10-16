static ssize_t tun_show_flags(struct device *dev, struct device_attribute *attr,
			      char *buf)
{
	struct tun_struct *tun = netdev_priv(to_net_dev(dev));
	return sprintf(buf, "0x%x\n", tun_flags(tun));
}