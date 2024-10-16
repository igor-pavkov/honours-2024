static int rfcomm_get_dev_info(void __user *arg)
{
	struct rfcomm_dev *dev;
	struct rfcomm_dev_info di;
	int err = 0;

	BT_DBG("");

	if (copy_from_user(&di, arg, sizeof(di)))
		return -EFAULT;

	dev = rfcomm_dev_get(di.id);
	if (!dev)
		return -ENODEV;

	di.flags   = dev->flags;
	di.channel = dev->channel;
	di.state   = dev->dlc->state;
	bacpy(&di.src, &dev->src);
	bacpy(&di.dst, &dev->dst);

	if (copy_to_user(arg, &di, sizeof(di)))
		err = -EFAULT;

	tty_port_put(&dev->port);
	return err;
}