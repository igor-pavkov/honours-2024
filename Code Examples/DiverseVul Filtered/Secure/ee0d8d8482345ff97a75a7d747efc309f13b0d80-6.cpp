static int __init ipx_init(void)
{
	int rc = proto_register(&ipx_proto, 1);

	if (rc != 0)
		goto out;

	sock_register(&ipx_family_ops);

	pEII_datalink = make_EII_client();
	if (pEII_datalink)
		dev_add_pack(&ipx_dix_packet_type);
	else
		printk(ipx_EII_err_msg);

	p8023_datalink = make_8023_client();
	if (p8023_datalink)
		dev_add_pack(&ipx_8023_packet_type);
	else
		printk(ipx_8023_err_msg);

	p8022_datalink = register_8022_client(ipx_8022_type, ipx_rcv);
	if (!p8022_datalink)
		printk(ipx_llc_err_msg);

	pSNAP_datalink = register_snap_client(ipx_snap_id, ipx_rcv);
	if (!pSNAP_datalink)
		printk(ipx_snap_err_msg);

	register_netdevice_notifier(&ipx_dev_notifier);
	ipx_register_sysctl();
	ipx_proc_init();
out:
	return rc;
}