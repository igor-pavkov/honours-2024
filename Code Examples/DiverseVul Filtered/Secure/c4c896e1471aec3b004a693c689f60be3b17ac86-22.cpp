static void __exit sco_exit(void)
{
	debugfs_remove(sco_debugfs);

	if (bt_sock_unregister(BTPROTO_SCO) < 0)
		BT_ERR("SCO socket unregistration failed");

	if (hci_unregister_proto(&sco_hci_proto) < 0)
		BT_ERR("SCO protocol unregistration failed");

	proto_unregister(&sco_proto);
}