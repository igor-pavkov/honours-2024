static int __init sco_init(void)
{
	int err;

	err = proto_register(&sco_proto, 0);
	if (err < 0)
		return err;

	err = bt_sock_register(BTPROTO_SCO, &sco_sock_family_ops);
	if (err < 0) {
		BT_ERR("SCO socket registration failed");
		goto error;
	}

	err = hci_register_proto(&sco_hci_proto);
	if (err < 0) {
		BT_ERR("SCO protocol registration failed");
		bt_sock_unregister(BTPROTO_SCO);
		goto error;
	}

	if (bt_debugfs) {
		sco_debugfs = debugfs_create_file("sco", 0444,
					bt_debugfs, NULL, &sco_debugfs_fops);
		if (!sco_debugfs)
			BT_ERR("Failed to create SCO debug file");
	}

	BT_INFO("SCO (Voice Link) ver %s", VERSION);
	BT_INFO("SCO socket layer initialized");

	return 0;

error:
	proto_unregister(&sco_proto);
	return err;
}