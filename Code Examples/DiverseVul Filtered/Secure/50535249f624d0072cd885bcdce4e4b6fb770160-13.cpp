static int __init qrtr_proto_init(void)
{
	int rc;

	rc = proto_register(&qrtr_proto, 1);
	if (rc)
		return rc;

	rc = sock_register(&qrtr_family);
	if (rc)
		goto err_proto;

	rc = qrtr_ns_init();
	if (rc)
		goto err_sock;

	return 0;

err_sock:
	sock_unregister(qrtr_family.family);
err_proto:
	proto_unregister(&qrtr_proto);
	return rc;
}