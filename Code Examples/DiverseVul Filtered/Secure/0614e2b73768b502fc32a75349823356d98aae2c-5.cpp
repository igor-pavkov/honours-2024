static int __init ax25_init(void)
{
	int rc = proto_register(&ax25_proto, 0);

	if (rc != 0)
		goto out;

	sock_register(&ax25_family_ops);
	dev_add_pack(&ax25_packet_type);
	register_netdevice_notifier(&ax25_dev_notifier);

	proc_create_seq("ax25_route", 0444, init_net.proc_net, &ax25_rt_seqops);
	proc_create_seq("ax25", 0444, init_net.proc_net, &ax25_info_seqops);
	proc_create_seq("ax25_calls", 0444, init_net.proc_net,
			&ax25_uid_seqops);
out:
	return rc;
}