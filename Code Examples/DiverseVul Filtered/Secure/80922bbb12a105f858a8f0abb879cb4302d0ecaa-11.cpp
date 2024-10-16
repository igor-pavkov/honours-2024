static void __exit econet_proto_exit(void)
{
#ifdef CONFIG_ECONET_AUNUDP
	del_timer(&ab_cleanup_timer);
	if (udpsock)
		sock_release(udpsock);
#endif
	unregister_netdevice_notifier(&econet_netdev_notifier);
#ifdef CONFIG_ECONET_NATIVE
	dev_remove_pack(&econet_packet_type);
#endif
	sock_unregister(econet_family_ops.family);
	proto_unregister(&econet_proto);
}