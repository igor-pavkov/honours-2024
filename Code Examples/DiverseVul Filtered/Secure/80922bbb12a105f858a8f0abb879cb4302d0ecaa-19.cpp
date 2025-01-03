static int ec_dev_ioctl(struct socket *sock, unsigned int cmd, void __user *arg)
{
	struct ifreq ifr;
	struct ec_device *edev;
	struct net_device *dev;
	struct sockaddr_ec *sec;
	int err;

	/*
	 *	Fetch the caller's info block into kernel space
	 */

	if (copy_from_user(&ifr, arg, sizeof(struct ifreq)))
		return -EFAULT;

	if ((dev = dev_get_by_name(&init_net, ifr.ifr_name)) == NULL)
		return -ENODEV;

	sec = (struct sockaddr_ec *)&ifr.ifr_addr;

	mutex_lock(&econet_mutex);

	err = 0;
	switch (cmd) {
	case SIOCSIFADDR:
		edev = dev->ec_ptr;
		if (edev == NULL) {
			/* Magic up a new one. */
			edev = kzalloc(sizeof(struct ec_device), GFP_KERNEL);
			if (edev == NULL) {
				err = -ENOMEM;
				break;
			}
			dev->ec_ptr = edev;
		} else
			net2dev_map[edev->net] = NULL;
		edev->station = sec->addr.station;
		edev->net = sec->addr.net;
		net2dev_map[sec->addr.net] = dev;
		if (!net2dev_map[0])
			net2dev_map[0] = dev;
		break;

	case SIOCGIFADDR:
		edev = dev->ec_ptr;
		if (edev == NULL) {
			err = -ENODEV;
			break;
		}
		memset(sec, 0, sizeof(struct sockaddr_ec));
		sec->addr.station = edev->station;
		sec->addr.net = edev->net;
		sec->sec_family = AF_ECONET;
		dev_put(dev);
		if (copy_to_user(arg, &ifr, sizeof(struct ifreq)))
			err = -EFAULT;
		break;

	default:
		err = -EINVAL;
		break;
	}

	mutex_unlock(&econet_mutex);

	dev_put(dev);

	return err;
}