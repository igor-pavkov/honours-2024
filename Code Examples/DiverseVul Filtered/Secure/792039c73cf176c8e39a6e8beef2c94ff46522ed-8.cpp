void l2cap_cleanup_sockets(void)
{
	if (bt_sock_unregister(BTPROTO_L2CAP) < 0)
		BT_ERR("L2CAP socket unregistration failed");

	proto_unregister(&l2cap_proto);
}