int __init irsock_init(void)
{
	int rc = proto_register(&irda_proto, 0);

	if (rc == 0)
		rc = sock_register(&irda_family_ops);

	return rc;
}