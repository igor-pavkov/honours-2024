int __init l2cap_init_sockets(void)
{
	int err;

	err = proto_register(&l2cap_proto, 0);
	if (err < 0)
		return err;

	err = bt_sock_register(BTPROTO_L2CAP, &l2cap_sock_family_ops);
	if (err < 0)
		goto error;

	BT_INFO("L2CAP socket layer initialized");

	return 0;

error:
	BT_ERR("L2CAP socket registration failed");
	proto_unregister(&l2cap_proto);
	return err;
}