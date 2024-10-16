void __exit rfcomm_cleanup_sockets(void)
{
	bt_procfs_cleanup(&init_net, "rfcomm");

	debugfs_remove(rfcomm_sock_debugfs);

	bt_sock_unregister(BTPROTO_RFCOMM);

	proto_unregister(&rfcomm_proto);
}