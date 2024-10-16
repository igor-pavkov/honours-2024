static int __init rose_proto_init(void)
{
	int i;
	int rc;

	if (rose_ndevs > 0x7FFFFFFF/sizeof(struct net_device *)) {
		printk(KERN_ERR "ROSE: rose_proto_init - rose_ndevs parameter to large\n");
		rc = -EINVAL;
		goto out;
	}

	rc = proto_register(&rose_proto, 0);
	if (rc != 0)
		goto out;

	rose_callsign = null_ax25_address;

	dev_rose = kzalloc(rose_ndevs * sizeof(struct net_device *), GFP_KERNEL);
	if (dev_rose == NULL) {
		printk(KERN_ERR "ROSE: rose_proto_init - unable to allocate device structure\n");
		rc = -ENOMEM;
		goto out_proto_unregister;
	}

	for (i = 0; i < rose_ndevs; i++) {
		struct net_device *dev;
		char name[IFNAMSIZ];

		sprintf(name, "rose%d", i);
		dev = alloc_netdev(0, name, rose_setup);
		if (!dev) {
			printk(KERN_ERR "ROSE: rose_proto_init - unable to allocate memory\n");
			rc = -ENOMEM;
			goto fail;
		}
		rc = register_netdev(dev);
		if (rc) {
			printk(KERN_ERR "ROSE: netdevice registration failed\n");
			free_netdev(dev);
			goto fail;
		}
		rose_set_lockdep_key(dev);
		dev_rose[i] = dev;
	}

	sock_register(&rose_family_ops);
	register_netdevice_notifier(&rose_dev_notifier);

	ax25_register_pid(&rose_pid);
	ax25_linkfail_register(&rose_linkfail_notifier);

#ifdef CONFIG_SYSCTL
	rose_register_sysctl();
#endif
	rose_loopback_init();

	rose_add_loopback_neigh();

	proc_net_fops_create(&init_net, "rose", S_IRUGO, &rose_info_fops);
	proc_net_fops_create(&init_net, "rose_neigh", S_IRUGO, &rose_neigh_fops);
	proc_net_fops_create(&init_net, "rose_nodes", S_IRUGO, &rose_nodes_fops);
	proc_net_fops_create(&init_net, "rose_routes", S_IRUGO, &rose_routes_fops);
out:
	return rc;
fail:
	while (--i >= 0) {
		unregister_netdev(dev_rose[i]);
		free_netdev(dev_rose[i]);
	}
	kfree(dev_rose);
out_proto_unregister:
	proto_unregister(&rose_proto);
	goto out;
}