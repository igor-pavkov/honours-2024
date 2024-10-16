static void __exit rose_exit(void)
{
	int i;

	proc_net_remove(&init_net, "rose");
	proc_net_remove(&init_net, "rose_neigh");
	proc_net_remove(&init_net, "rose_nodes");
	proc_net_remove(&init_net, "rose_routes");
	rose_loopback_clear();

	rose_rt_free();

	ax25_protocol_release(AX25_P_ROSE);
	ax25_linkfail_release(&rose_linkfail_notifier);

	if (ax25cmp(&rose_callsign, &null_ax25_address) != 0)
		ax25_listen_release(&rose_callsign, NULL);

#ifdef CONFIG_SYSCTL
	rose_unregister_sysctl();
#endif
	unregister_netdevice_notifier(&rose_dev_notifier);

	sock_unregister(PF_ROSE);

	for (i = 0; i < rose_ndevs; i++) {
		struct net_device *dev = dev_rose[i];

		if (dev) {
			unregister_netdev(dev);
			free_netdev(dev);
		}
	}

	kfree(dev_rose);
	proto_unregister(&rose_proto);
}