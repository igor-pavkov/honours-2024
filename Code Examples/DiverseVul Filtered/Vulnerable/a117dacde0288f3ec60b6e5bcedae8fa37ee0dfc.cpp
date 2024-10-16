static long __tun_chr_ioctl(struct file *file, unsigned int cmd,
			    unsigned long arg, int ifreq_len)
{
	struct tun_file *tfile = file->private_data;
	struct tun_struct *tun;
	void __user* argp = (void __user*)arg;
	struct sock_fprog fprog;
	struct ifreq ifr;
	int sndbuf;
	int vnet_hdr_sz;
	int ret;

	if (cmd == TUNSETIFF || _IOC_TYPE(cmd) == 0x89)
		if (copy_from_user(&ifr, argp, ifreq_len))
			return -EFAULT;

	if (cmd == TUNGETFEATURES) {
		/* Currently this just means: "what IFF flags are valid?".
		 * This is needed because we never checked for invalid flags on
		 * TUNSETIFF. */
		return put_user(IFF_TUN | IFF_TAP | IFF_NO_PI | IFF_ONE_QUEUE |
				IFF_VNET_HDR,
				(unsigned int __user*)argp);
	}

	rtnl_lock();

	tun = __tun_get(tfile);
	if (cmd == TUNSETIFF && !tun) {
		ifr.ifr_name[IFNAMSIZ-1] = '\0';

		ret = tun_set_iff(tfile->net, file, &ifr);

		if (ret)
			goto unlock;

		if (copy_to_user(argp, &ifr, ifreq_len))
			ret = -EFAULT;
		goto unlock;
	}

	ret = -EBADFD;
	if (!tun)
		goto unlock;

	tun_debug(KERN_INFO, tun, "tun_chr_ioctl cmd %d\n", cmd);

	ret = 0;
	switch (cmd) {
	case TUNGETIFF:
		ret = tun_get_iff(current->nsproxy->net_ns, tun, &ifr);
		if (ret)
			break;

		if (copy_to_user(argp, &ifr, ifreq_len))
			ret = -EFAULT;
		break;

	case TUNSETNOCSUM:
		/* Disable/Enable checksum */

		/* [unimplemented] */
		tun_debug(KERN_INFO, tun, "ignored: set checksum %s\n",
			  arg ? "disabled" : "enabled");
		break;

	case TUNSETPERSIST:
		/* Disable/Enable persist mode */
		if (arg)
			tun->flags |= TUN_PERSIST;
		else
			tun->flags &= ~TUN_PERSIST;

		tun_debug(KERN_INFO, tun, "persist %s\n",
			  arg ? "enabled" : "disabled");
		break;

	case TUNSETOWNER:
		/* Set owner of the device */
		tun->owner = (uid_t) arg;

		tun_debug(KERN_INFO, tun, "owner set to %d\n", tun->owner);
		break;

	case TUNSETGROUP:
		/* Set group of the device */
		tun->group= (gid_t) arg;

		tun_debug(KERN_INFO, tun, "group set to %d\n", tun->group);
		break;

	case TUNSETLINK:
		/* Only allow setting the type when the interface is down */
		if (tun->dev->flags & IFF_UP) {
			tun_debug(KERN_INFO, tun,
				  "Linktype set failed because interface is up\n");
			ret = -EBUSY;
		} else {
			tun->dev->type = (int) arg;
			tun_debug(KERN_INFO, tun, "linktype set to %d\n",
				  tun->dev->type);
			ret = 0;
		}
		break;

#ifdef TUN_DEBUG
	case TUNSETDEBUG:
		tun->debug = arg;
		break;
#endif
	case TUNSETOFFLOAD:
		ret = set_offload(tun, arg);
		break;

	case TUNSETTXFILTER:
		/* Can be set only for TAPs */
		ret = -EINVAL;
		if ((tun->flags & TUN_TYPE_MASK) != TUN_TAP_DEV)
			break;
		ret = update_filter(&tun->txflt, (void __user *)arg);
		break;

	case SIOCGIFHWADDR:
		/* Get hw address */
		memcpy(ifr.ifr_hwaddr.sa_data, tun->dev->dev_addr, ETH_ALEN);
		ifr.ifr_hwaddr.sa_family = tun->dev->type;
		if (copy_to_user(argp, &ifr, ifreq_len))
			ret = -EFAULT;
		break;

	case SIOCSIFHWADDR:
		/* Set hw address */
		tun_debug(KERN_DEBUG, tun, "set hw address: %pM\n",
			  ifr.ifr_hwaddr.sa_data);

		ret = dev_set_mac_address(tun->dev, &ifr.ifr_hwaddr);
		break;

	case TUNGETSNDBUF:
		sndbuf = tun->socket.sk->sk_sndbuf;
		if (copy_to_user(argp, &sndbuf, sizeof(sndbuf)))
			ret = -EFAULT;
		break;

	case TUNSETSNDBUF:
		if (copy_from_user(&sndbuf, argp, sizeof(sndbuf))) {
			ret = -EFAULT;
			break;
		}

		tun->socket.sk->sk_sndbuf = sndbuf;
		break;

	case TUNGETVNETHDRSZ:
		vnet_hdr_sz = tun->vnet_hdr_sz;
		if (copy_to_user(argp, &vnet_hdr_sz, sizeof(vnet_hdr_sz)))
			ret = -EFAULT;
		break;

	case TUNSETVNETHDRSZ:
		if (copy_from_user(&vnet_hdr_sz, argp, sizeof(vnet_hdr_sz))) {
			ret = -EFAULT;
			break;
		}
		if (vnet_hdr_sz < (int)sizeof(struct virtio_net_hdr)) {
			ret = -EINVAL;
			break;
		}

		tun->vnet_hdr_sz = vnet_hdr_sz;
		break;

	case TUNATTACHFILTER:
		/* Can be set only for TAPs */
		ret = -EINVAL;
		if ((tun->flags & TUN_TYPE_MASK) != TUN_TAP_DEV)
			break;
		ret = -EFAULT;
		if (copy_from_user(&fprog, argp, sizeof(fprog)))
			break;

		ret = sk_attach_filter(&fprog, tun->socket.sk);
		break;

	case TUNDETACHFILTER:
		/* Can be set only for TAPs */
		ret = -EINVAL;
		if ((tun->flags & TUN_TYPE_MASK) != TUN_TAP_DEV)
			break;
		ret = sk_detach_filter(tun->socket.sk);
		break;

	default:
		ret = -EINVAL;
		break;
	}

unlock:
	rtnl_unlock();
	if (tun)
		tun_put(tun);
	return ret;
}