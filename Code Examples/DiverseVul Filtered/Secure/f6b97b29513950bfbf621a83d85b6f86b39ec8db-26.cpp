static int __init nr_proto_init(void)
{
	int i;
	int rc = proto_register(&nr_proto, 0);

	if (rc != 0)
		goto out;

	if (nr_ndevs > 0x7fffffff/sizeof(struct net_device *)) {
		printk(KERN_ERR "NET/ROM: nr_proto_init - nr_ndevs parameter to large\n");
		return -1;
	}

	dev_nr = kzalloc(nr_ndevs * sizeof(struct net_device *), GFP_KERNEL);
	if (dev_nr == NULL) {
		printk(KERN_ERR "NET/ROM: nr_proto_init - unable to allocate device array\n");
		return -1;
	}

	for (i = 0; i < nr_ndevs; i++) {
		char name[IFNAMSIZ];
		struct net_device *dev;

		sprintf(name, "nr%d", i);
		dev = alloc_netdev(0, name, nr_setup);
		if (!dev) {
			printk(KERN_ERR "NET/ROM: nr_proto_init - unable to allocate device structure\n");
			goto fail;
		}

		dev->base_addr = i;
		if (register_netdev(dev)) {
			printk(KERN_ERR "NET/ROM: nr_proto_init - unable to register network device\n");
			free_netdev(dev);
			goto fail;
		}
		nr_set_lockdep_key(dev);
		dev_nr[i] = dev;
	}

	if (sock_register(&nr_family_ops)) {
		printk(KERN_ERR "NET/ROM: nr_proto_init - unable to register socket family\n");
		goto fail;
	}

	register_netdevice_notifier(&nr_dev_notifier);

	ax25_register_pid(&nr_pid);
	ax25_linkfail_register(&nr_linkfail_notifier);

#ifdef CONFIG_SYSCTL
	nr_register_sysctl();
#endif

	nr_loopback_init();

	proc_net_fops_create(&init_net, "nr", S_IRUGO, &nr_info_fops);
	proc_net_fops_create(&init_net, "nr_neigh", S_IRUGO, &nr_neigh_fops);
	proc_net_fops_create(&init_net, "nr_nodes", S_IRUGO, &nr_nodes_fops);
out:
	return rc;
fail:
	while (--i >= 0) {
		unregister_netdev(dev_nr[i]);
		free_netdev(dev_nr[i]);
	}
	kfree(dev_nr);
	proto_unregister(&nr_proto);
	rc = -1;
	goto out;
}