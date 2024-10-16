static int atrtr_ioctl(unsigned int cmd, void __user *arg)
{
	struct rtentry rt;

	if (copy_from_user(&rt, arg, sizeof(rt)))
		return -EFAULT;

	switch (cmd) {
		case SIOCDELRT:
			if (rt.rt_dst.sa_family != AF_APPLETALK)
				return -EINVAL;
			return atrtr_delete(&((struct sockaddr_at *)
						&rt.rt_dst)->sat_addr);

		case SIOCADDRT: {
			struct net_device *dev = NULL;
			if (rt.rt_dev) {
				char name[IFNAMSIZ];
				if (copy_from_user(name, rt.rt_dev, IFNAMSIZ-1))
					return -EFAULT;
				name[IFNAMSIZ-1] = '\0';
				dev = __dev_get_by_name(&init_net, name);
				if (!dev)
					return -ENODEV;
			}
			return atrtr_create(&rt, dev);
		}
	}
	return -EINVAL;
}