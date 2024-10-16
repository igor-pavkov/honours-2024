static int __init econet_proto_init(void)
{
	int err = proto_register(&econet_proto, 0);

	if (err != 0)
		goto out;
	sock_register(&econet_family_ops);
#ifdef CONFIG_ECONET_AUNUDP
	spin_lock_init(&aun_queue_lock);
	aun_udp_initialise();
#endif
#ifdef CONFIG_ECONET_NATIVE
	econet_hw_initialise();
#endif
	register_netdevice_notifier(&econet_netdev_notifier);
out:
	return err;
}