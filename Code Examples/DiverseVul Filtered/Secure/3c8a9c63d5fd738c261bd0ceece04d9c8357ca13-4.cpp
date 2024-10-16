static int tun_chr_ioctl(struct inode *inode, struct file *file,
			 unsigned int cmd, unsigned long arg)
{
	struct tun_file *tfile = file->private_data;
	struct tun_struct *tun;
	void __user* argp = (void __user*)arg;
	struct ifreq ifr;
	int sndbuf;
	int ret;

	if (cmd == TUNSETIFF || _IOC_TYPE(cmd) == 0x89)
		if (copy_from_user(&ifr, argp, sizeof ifr))
			return -EFAULT;

	if (cmd == TUNGETFEATURES) {
		/* Currently this just means: "what IFF flags are valid?".
		 * This is needed because we never checked for invalid flags on
		 * TUNSETIFF. */
		return put_user(IFF_TUN | IFF_TAP | IFF_NO_PI | IFF_ONE_QUEUE |
				IFF_VNET_HDR,
				(unsigned int __user*)argp);
	}

	tun = __tun_get(tfile);
	if (cmd == TUNSETIFF && !tun) {
		int err;

		ifr.ifr_name[IFNAMSIZ-1] = '\0';

		rtnl_lock();
		err = tun_set_iff(tfile->net, file, &ifr);
		rtnl_unlock();

		if (err)
			return err;

		if (copy_to_user(argp, &ifr, sizeof(ifr)))
			return -EFAULT;
		return 0;
	}


	if (!tun)
		return -EBADFD;

	DBG(KERN_INFO "%s: tun_chr_ioctl cmd %d\n", tun->dev->name, cmd);

	ret = 0;
	switch (cmd) {
	case TUNGETIFF:
		ret = tun_get_iff(current->nsproxy->net_ns, file, &ifr);
		if (ret)
			break;

		if (copy_to_user(argp, &ifr, sizeof(ifr)))
			ret = -EFAULT;
		break;

	case TUNSETNOCSUM:
		/* Disable/Enable checksum */
		if (arg)
			tun->flags |= TUN_NOCHECKSUM;
		else
			tun->flags &= ~TUN_NOCHECKSUM;

		DBG(KERN_INFO "%s: checksum %s\n",
		    tun->dev->name, arg ? "disabled" : "enabled");
		break;

	case TUNSETPERSIST:
		/* Disable/Enable persist mode */
		if (arg)
			tun->flags |= TUN_PERSIST;
		else
			tun->flags &= ~TUN_PERSIST;

		DBG(KERN_INFO "%s: persist %s\n",
		    tun->dev->name, arg ? "enabled" : "disabled");
		break;

	case TUNSETOWNER:
		/* Set owner of the device */
		tun->owner = (uid_t) arg;

		DBG(KERN_INFO "%s: owner set to %d\n", tun->dev->name, tun->owner);
		break;

	case TUNSETGROUP:
		/* Set group of the device */
		tun->group= (gid_t) arg;

		DBG(KERN_INFO "%s: group set to %d\n", tun->dev->name, tun->group);
		break;

	case TUNSETLINK:
		/* Only allow setting the type when the interface is down */
		rtnl_lock();
		if (tun->dev->flags & IFF_UP) {
			DBG(KERN_INFO "%s: Linktype set failed because interface is up\n",
				tun->dev->name);
			ret = -EBUSY;
		} else {
			tun->dev->type = (int) arg;
			DBG(KERN_INFO "%s: linktype set to %d\n", tun->dev->name, tun->dev->type);
			ret = 0;
		}
		rtnl_unlock();
		break;

#ifdef TUN_DEBUG
	case TUNSETDEBUG:
		tun->debug = arg;
		break;
#endif
	case TUNSETOFFLOAD:
		rtnl_lock();
		ret = set_offload(tun->dev, arg);
		rtnl_unlock();
		break;

	case TUNSETTXFILTER:
		/* Can be set only for TAPs */
		ret = -EINVAL;
		if ((tun->flags & TUN_TYPE_MASK) != TUN_TAP_DEV)
			break;
		rtnl_lock();
		ret = update_filter(&tun->txflt, (void __user *)arg);
		rtnl_unlock();
		break;

	case SIOCGIFHWADDR:
		/* Get hw addres */
		memcpy(ifr.ifr_hwaddr.sa_data, tun->dev->dev_addr, ETH_ALEN);
		ifr.ifr_hwaddr.sa_family = tun->dev->type;
		if (copy_to_user(argp, &ifr, sizeof ifr))
			ret = -EFAULT;
		break;

	case SIOCSIFHWADDR:
		/* Set hw address */
		DBG(KERN_DEBUG "%s: set hw address: %pM\n",
			tun->dev->name, ifr.ifr_hwaddr.sa_data);

		rtnl_lock();
		ret = dev_set_mac_address(tun->dev, &ifr.ifr_hwaddr);
		rtnl_unlock();
		break;

	case TUNGETSNDBUF:
		sndbuf = tun->sk->sk_sndbuf;
		if (copy_to_user(argp, &sndbuf, sizeof(sndbuf)))
			ret = -EFAULT;
		break;

	case TUNSETSNDBUF:
		if (copy_from_user(&sndbuf, argp, sizeof(sndbuf))) {
			ret = -EFAULT;
			break;
		}

		tun->sk->sk_sndbuf = sndbuf;
		break;

	default:
		ret = -EINVAL;
		break;
	};

	tun_put(tun);
	return ret;
}