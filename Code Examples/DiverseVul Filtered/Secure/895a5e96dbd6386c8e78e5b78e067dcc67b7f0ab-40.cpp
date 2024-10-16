static ssize_t netdev_show(const struct device *dev,
			   struct device_attribute *attr, char *buf,
			   ssize_t (*format)(const struct net_device *, char *))
{
	struct net_device *ndev = to_net_dev(dev);
	ssize_t ret = -EINVAL;

	read_lock(&dev_base_lock);
	if (dev_isalive(ndev))
		ret = (*format)(ndev, buf);
	read_unlock(&dev_base_lock);

	return ret;
}